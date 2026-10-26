/**
 * @file example.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief
 *
 * @author s3mat3
 */

#include <iostream>
#include <thread>
#include <chrono>
#include "timer.hpp"

using namespace std::chrono_literals;

int main() {
    moto::Timer timer;

    // 1. Simple measurement (start/stop)
    std::cout << "--- Simple Measurement ---" << std::endl;
    timer.start();
    std::this_thread::sleep_for(123ms);
    auto elapsed = timer.stop();
    std::cout << "Elapsed: " << elapsed.count() << " ns ("
              << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
              << " ms)" << std::endl;

    // 2. Timeout check (One-shot)
    std::cout << "\n--- One-shot Timeout (500ms) ---" << std::endl;
    timer.start();
    while (true) {
        if (timer.isTimeout(500ms, false)) {
            std::cout << "Timeout occurred! Timer stopped." << std::endl;
            break;
        }
        std::cout << "Waiting..." << std::endl;
        std::this_thread::sleep_for(100ms);
    }
    std::cout << "Timer running status: " << ((timer) ? "Running" : "Stopped") << std::endl;

    // 3. Repeat timeout check (Repeatable)
    std::cout << "\n--- Repeatable Timeout (200ms) ---" << std::endl;
    timer.start();
    for (int i = 0; i < 5; ++i) {
        if (timer.isTimeout(200ms, true)) {
            std::cout << "Tick " << (i + 1) << " occurred!" << std::endl;
        }
        std::this_thread::sleep_for(50ms);
    }

    // 4. Suspend/Resume
    std::cout << "\n--- Suspend and Resume ---" << std::endl;
    timer.start();
    std::this_thread::sleep_for(100ms);
    std::cout << "Suspending timer..." << std::endl;
    timer.suspend();

    // isTimeout returns false while suspending
    if (!timer.isTimeout(10ms)) {
        std::cout << "Timer is suspended, isTimeout returned false." << std::endl;
    }

    std::cout << "Resuming timer..." << std::endl;
    timer.resume();
    if (timer.isTimeout(10ms)) {
        std::cout << "Timer resumed and timeout detected." << std::endl;
    }

    return 0;
}


