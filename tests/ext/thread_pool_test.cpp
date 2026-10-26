/**
 * @file thread_pool_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for thread_pool.hpp
 *
 * @author s3mat3
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ext/thread_pool.hpp"
#include "result.hpp"
#include <chrono>
#include <string>
#include <atomic>

TEST_CASE("ThreadPool Basic Functionality") {
    moto::ThreadPool pool(4);

    SUBCASE("Execute simple task with return value") {
        auto future = pool.enqueue([](int a, int b) {
            return moto::Result<int>(a + b);
        }, 10, 20);

        auto result = future.get();
        REQUIRE(result.has_value()); // Use REQUIRE to stop if no value

        int val = result.value();
        CHECK(val == 30);
    }

    SUBCASE("Execute void task") {
        std::atomic<bool> executed{false};
        auto future = pool.enqueue([&executed]() {
            executed = true;
            return moto::Result<void>();
        });

        auto result = future.get();
        CHECK(result.has_value());
        CHECK(executed == true);
    }
    SUBCASE("Handle multiple concurrent tasks") {
        std::atomic<int> counter{0};
        const int num_tasks = 100;
        std::vector<std::future<moto::Result<void>>> futures;

        for (int i = 0; i < num_tasks; ++i) {
            futures.push_back(pool.enqueue([&counter]() {
                counter++;
                return moto::Result<void>();
            }));
        }

        for (auto& f : futures) {
            f.get();
        }
        CHECK(counter == num_tasks);
    }
}

TEST_CASE("ThreadPool Error Handling and Resilience") {
    moto::ThreadPool pool(2);

    SUBCASE("Explicit Error return via Result") {
        auto future = pool.enqueue([]() {
            return moto::Result<int>(moto::Error(404, "Not Found"));
        });

        auto result = future.get();
        CHECK_FALSE(result.has_value());
        CHECK(result.error().code == 404);
        CHECK(result.error().cause == "Not Found");
    }

    SUBCASE("Recovery from uncaught exception in task") {
        // This task throws a raw exception which should be caught by ThreadPool 
        // and converted into a moto::Error Result.
        auto future = pool.enqueue([]() -> moto::Result<int> {
            throw std::runtime_error("Unexpected crash");
            return moto::Result<int>(1);
        });

        auto result = future.get();
        CHECK_FALSE(result.has_value());
        CHECK(result.error().cause == "Unexpected crash");
    }

    SUBCASE("Recovery from non-std::exception throw") {
        auto future = pool.enqueue([]() -> moto::Result<int> {
            throw 42; // Throwing an int
            return moto::Result<int>(1);
        });

        auto result = future.get();
        CHECK_FALSE(result.has_value());
        CHECK(result.error().cause == "Unknown exception occurred");
    }
}

TEST_CASE("ThreadPool Lifecycle and Edge Cases") {
    SUBCASE("Hardware concurrency fallback") {
        // Testing with 0 should fallback to 1 thread internally
        moto::ThreadPool pool(0);
        CHECK(pool.size() == 1);
    }

    SUBCASE("Enqueue after stop") {
        auto pool = std::make_unique<moto::ThreadPool>(2);
        pool->stop();

        auto future = pool->enqueue([]() {
            return moto::Result<int>(1);
        });

        auto result = future.get();
        CHECK_FALSE(result.has_value());
        CHECK(result.error().cause == "ThreadPool is stopped");
    }

    SUBCASE("Task execution during shutdown") {
        std::atomic<bool> task_ran{false};
        {
            moto::ThreadPool pool(1);
            pool.enqueue([&task_ran]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                task_ran = true;
                return moto::Result<void>();
            });
            // Pool goes out of scope here, destructor calls stop()
            // Workers should finish the remaining queue before exiting
        }
        CHECK(task_ran == true);
    }
}

