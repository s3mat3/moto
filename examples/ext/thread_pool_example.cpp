/**
 * @file example_thread_pool.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief
 *
 * @author s3mat3
 */
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "result.hpp"
#include "thread_pool.hpp"

/**
 * @brief A sample function that performs a heavy computation.
 * 
 * @param id The task ID.
 * @param duration_ms Simulation of work duration in milliseconds.
 * @return An integer result (the square of the ID).
 * @throws std::runtime_error if the ID is negative.
 */
int heavy_computation(int id, int duration_ms) {
    if (id < 0) {
        throw std::runtime_error("Negative ID is not allowed");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    return id * id;
}

/**
 * @brief A sample function that already returns a moto::Result.
 * 
 * @param val The value to check.
 * @return Result containing the value if positive, otherwise an Error.
 */
moto::Result<int> check_positive(int val) {
    if (val > 0) {
        return moto::Result<int>(val);
    }
    return moto::Result<int>(moto::Error(400, "Value must be positive"));
}

/**
 * @brief A sample function that returns Result<void>.
 * 
 * @param name The name to print.
 * @throws std::invalid_argument if name is empty.
 */
void print_message(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Name cannot be empty");
    }
    std::cout << "[Worker] Hello, " << name << "!\n";
}

int main() {
    // Initialize ThreadPool with 4 worker threads
    moto::ThreadPool pool(4);
    
    // Storage for futures of different return types
    std::vector<std::future<moto::Result<int>>> int_results;
    std::vector<std::future<moto::Result<void>>> void_results;

    // 1. Enqueue tasks that return raw values
    for (int i = 0; i < 3; ++i) {
        int_results.emplace_back(
            pool.enqueue([i]() { return heavy_computation(i, 100); })
        );
    }

    // 2. Enqueue a task that throws an exception
    int_results.emplace_back(
        pool.enqueue([]() { return heavy_computation(-1, 100); })
    );

    // 3. Enqueue tasks that already return moto::Result<int>
    int_results.emplace_back(
        pool.enqueue([]() { return check_positive(10); })
    );
    int_results.emplace_back(
        pool.enqueue([]() { return check_positive(-5); })
    );

    // 4. Enqueue tasks that return void
    void_results.emplace_back(
        pool.enqueue([]() { print_message("Fugaaaaa"); })
    );
    void_results.emplace_back(
        pool.enqueue([]() { print_message(""); }) // Will throw exception
    );
    // Process integer results
    std::cout << "--- Integer Task Results ---\n";
    for (size_t i = 0; i < int_results.size(); ++i) {
        auto res = int_results[i].get();
        if (res) { // Use operator bool()
            std::cout << "Task " << i << " succeeded: " << res.value() << "\n";
        } else {
            std::cout << "Task " << i << " failed: [" << res.error().code 
                      << "] " << res.error().cause << "\n";
        }
    }

    // Process void results
    std::cout << "\n--- Void Task Results ---\n";
    for (size_t i = 0; i < void_results.size(); ++i) {
        auto res = void_results[i].get();
        if (res.has_value()) {
            std::cout << "Void Task " << i << " completed successfully.\n";
        } else {
            std::cout << "Void Task " << i << " failed: " << res.error().cause << "\n";
        }
    }

    return 0;
}
