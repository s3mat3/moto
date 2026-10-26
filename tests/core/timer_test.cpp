/**
 * @file timer_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for timer.hpp
 *
 * @author s3mat3
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "timer.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

TEST_CASE("moto::Timer Basic Functionality") {
    moto::Timer timer;

    SUBCASE("Initial state should be not running") {
        CHECK_FALSE(static_cast<bool>(timer));
    }

    SUBCASE("Start and Stop") {
        timer.start();
        CHECK(static_cast<bool>(timer));

        std::this_thread::sleep_for(10ms);
        auto elapsed = timer.stop();

        CHECK_FALSE(static_cast<bool>(timer));
        CHECK(elapsed >= 10ms);
    }

    SUBCASE("Suspend and Resume") {
        timer.start();
        timer.suspend();
        CHECK_FALSE(static_cast<bool>(timer));

        timer.resume();
        CHECK(static_cast<bool>(timer));
    }
}

TEST_CASE("moto::Timer Timeout Logic") {
    moto::Timer timer;

    SUBCASE("isTimeout returns false when not running") {
        CHECK_FALSE(timer.isTimeout(10ms));
    }

    SUBCASE("One-shot timeout (repeatable = false)") {
        timer.start();

        // Before timeout
        CHECK_FALSE(timer.isTimeout(100ms, false));

        // wait for time to pass
        std::this_thread::sleep_for(110ms);

        // Timeout occurred
        CHECK(timer.isTimeout(100ms, false));

        // Since it is a one-shot, it should have stopped.
        CHECK_FALSE(static_cast<bool>(timer));
        // The second time it should be false
        CHECK_FALSE(timer.isTimeout(100ms, false));
    }

    SUBCASE("Repeatable timeout (repeatable = true)") {
        timer.start();

        // 1st timeout
        std::this_thread::sleep_for(110ms);
        CHECK(timer.isTimeout(100ms, true));

        // It is repeatable so it should continue to work
        CHECK(static_cast<bool>(timer));

        // If you call it immediately, the next timeout has not yet been reached.
        CHECK_FALSE(timer.isTimeout(100ms, true));

        // Waiting for second timeout
        std::this_thread::sleep_for(110ms);
        CHECK(timer.isTimeout(100ms, true));
    }
}

TEST_CASE("moto::Timer Precision and Types") {
    moto::Timer timer;

    SUBCASE("Different duration types") {
        timer.start();
        std::this_thread::sleep_for(20ms);

        // Judgment by second specification
        CHECK_FALSE(timer.isTimeout(1s));
        // Judgment by specifying milliseconds
        CHECK(timer.isTimeout(10ms));
    }
}

TEST_CASE("Timer Edge Cases") {
   moto::Timer timer;

    SUBCASE("isTimeout on non-running timer") {
        // If called without start()
        CHECK_FALSE(timer.isTimeout(1ms));
    }

    SUBCASE("Stop returns duration since last start") {
        timer.start();
        std::this_thread::sleep_for(5ms);
        timer.start(); // reset (restart)
        std::this_thread::sleep_for(5ms);
        auto elapsed = timer.stop();

        // Only the time from the second start should be measured.
        CHECK(elapsed >= 5ms);
        CHECK(elapsed < 10ms);
    }
}
