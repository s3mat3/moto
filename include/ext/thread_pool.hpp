/**
 * @file thread_pool.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief A robust thread pool implementation using Result for error handling.
 *
 * @author s3mat3
 */

#pragma once

#ifndef MOTO_THREAD_POOL_HPP
# define MOTO_THREAD_POOL_HPP

# include <vector>
# include <thread>
# include <queue>
# include <functional>
# include <mutex>
# include <condition_variable>
# include <atomic>
# include <future>
# include <memory>
# include "../core/result.hpp"

namespace moto {

/**
 * @brief Helper trait to extract the underlying value type from a Result or a raw type.
 */
template <typename T>
struct result_value_traits {
    using type = T;
};

template <typename T, typename E>
struct result_value_traits<Result<T, E>> {
    using type = T;
};

// Specialized trait for Result<void, E>
template <typename E>
struct result_value_traits<Result<void, E>> {
    using type = void;
};

template <typename T>
using result_value_t = typename result_value_traits<T>::type;

class ThreadPool {
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_{false};

public:
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency()) {
        size_t threads = (num_threads == 0) ? 1 : num_threads;
        for (size_t i = 0; i < threads; ++i) {
            workers_.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        condition_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() { stop(); }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void stop() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) worker.join();
        }
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) {
        // 1. Determine the raw return type of the callable
        using RawReturnType = std::invoke_result_t<F, Args...>;

        // 2. Use the helper trait to get the value type safely without causing compiler errors on non-class types
        using FinalValueType = result_value_t<RawReturnType>;
        using FinalResultType = Result<FinalValueType>;

        auto task = std::make_shared<std::packaged_task<FinalResultType()>>(
            [f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable -> FinalResultType {
                try {
                    if constexpr (std::is_void_v<RawReturnType>) {
                        std::invoke(f, args...);
                        return FinalResultType();
                    } else if constexpr (is_result_v<RawReturnType>) {
                        return std::invoke(f, args...);
                    } else {
                        return FinalResultType(std::invoke(f, args...));
                    }
                } catch (const std::exception& e) {
                    return FinalResultType(Error(-1, e.what()));
                } catch (...) {
                    return FinalResultType(Error(-1, "Unknown exception occurred"));
                }
            }
        );

        std::future<FinalResultType> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (stop_) {
                std::promise<FinalResultType> p;
                p.set_value(FinalResultType(Error(-2, "ThreadPool is stopped")));
                return p.get_future();
            }
            tasks_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return res;
    }

    size_t size() const { return workers_.size(); }
};

} // namespace moto
#endif
