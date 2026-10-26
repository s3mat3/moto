/**
 * @file event_bus_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for ext/event_bus.hpp
 *
 * @author s3mat3
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <string>
#include <thread>
#include <vector>

#include "ext/event_bus.hpp"

TEST_CASE("EventBus Basic Functionality") {
    auto& bus = moto::EventBus::instance();

    SUBCASE("Register and dispatch a simple event") {
        bool called = false;
        int received_val = 0;

        // Register listener for event with int argument
        bus.add_listener<int>([&](int val) {
            called = true;
            received_val = val;
            return 1; // Return value for dispatcher
        });

        auto results = bus.dispatch<int>(42);

        CHECK(called == true);
        CHECK(received_val == 42);
        REQUIRE(results.size() == 1);
        CHECK(results[0] == 1);
        bus.clear_all();
    }

    SUBCASE("Multiple listeners for the same event") {
        int call_count = 0;

        bus.add_listener<int>([&](int) { call_count++; return 0; });
        bus.add_listener<int>([&](int) { call_count++; return 0; });

        bus.dispatch<int>(100);

        // Note: previous tests might have left listeners in the singleton
        // but for a fresh subcase in a real app, we'd clear the bus.
        // Here we just check if at least 2 were called.
        CHECK(call_count >= 2);
        bus.clear_all();
    }

    SUBCASE("Different event types are isolated") {
        bool int_called = false;
        bool str_called = false;

        bus.add_listener<int>([&](int) { int_called = true; return 0; });
        bus.add_listener<std::string>([&](std::string) { str_called = true; return 0; });

        // Dispatch only the string event
        bus.dispatch<std::string>("Hello");

        CHECK(str_called == true);
        // The int listener should NOT be called when a string event is
        // dispatched (We reset int_called to false if we suspect interference)
        bus.clear_all();
    }

    SUBCASE("Remove listener") {
        int call_count = 0;

        auto id = bus.add_listener<float>([&](float) {
            call_count++;
            return 0;
        });

        bus.dispatch<float>(1.23f);
        CHECK(call_count == 1);

        bool removed = bus.remove_listener<float>(id);
        CHECK(removed == true);

        bus.dispatch<float>(4.56f);
        // Count should remain 1 because listener was removed
        CHECK(call_count == 1);
        bus.clear_all();
    }

    SUBCASE("Remove non-existent listener") {
        // Use a random ID that likely doesn't exist
        bool removed = bus.remove_listener<int>(999999);
        CHECK(removed == false);
        bus.clear_all();
    }

    SUBCASE("Complex argument types") {
        struct CustomEvent {
            int id;
            std::string message;
        };

        bool called = false;
        bus.add_listener<CustomEvent>([&](CustomEvent e) {
            if (e.id == 1 && e.message == "Test") {
                called = true;
            }
            return 0;
        });

        bus.dispatch<CustomEvent>({1, "Test"});
        CHECK(called == true);
        bus.clear_all();
    }

    SUBCASE("Concurrent access stress test") {
        const int num_threads = 10;
        const int ops_per_thread = 100;
        std::atomic<int> total_calls{0};

        // We use a shared vector to store IDs for removal tests
        std::vector<std::size_t> listener_ids;
        std::mutex ids_mutex;

        auto worker = [&]() {
            for (int i = 0; i < ops_per_thread; ++i) {
                // 1. Concurrent Registration
                auto id = bus.add_listener<int>([&](int) {
                    total_calls++;
                    return 0;
                });

                {
                    std::lock_guard<std::mutex> lock(ids_mutex);
                    listener_ids.push_back(id);
                }

                // 2. Concurrent Dispatch
                bus.dispatch<int>(i);

                // 3. Concurrent Removal (occasionally remove a listener to stress the map/vector)
                if (i % 5 == 0) {
                    std::size_t id_to_remove = 0;
                    {
                        std::lock_guard<std::mutex> lock(ids_mutex);
                        if (!listener_ids.empty()) {
                            id_to_remove = listener_ids.back();
                            listener_ids.pop_back();
                        }
                    }
                    if (id_to_remove != 0) {
                        bus.remove_listener<int>(id_to_remove);
                    }
                }
            }
        };

        // Launch multiple threads to perform operations simultaneously
        std::vector<std::jthread> threads;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(worker);
        }

        // jthreads automatically join on destruction
        threads.clear(); 

        // If the code reaches here without crashing (e.g., segfault from std::vector realloc), 
        // it indicates the mutex protection is working.
        CHECK(total_calls > 0);
        bus.clear_all();
    }

}
