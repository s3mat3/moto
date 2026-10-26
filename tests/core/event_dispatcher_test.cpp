/**
 * @file event_dispatcher_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for event_dispatcher.hpp
 *
 * @author s3mat3
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "event_dispatcher.hpp"

TEST_CASE("EventDispatcher Basic Functionality") {
    moto::EventDispatcher<int, double> dispatcher;

    SUBCASE("Adding and dispatching events") {
        int call_count_1 = 0;
        int call_count_2 = 0;

        auto id1 = dispatcher.add_listener([&](int a, double b) {
            call_count_1++;
            return a + static_cast<int>(b);
        });

        auto id2 = dispatcher.add_listener([&](int a, double) {
            call_count_2++;
            return a * 2;
        });

        CHECK(id1 != id2);

        auto results = dispatcher.dispatch(10, 5.5);

        CHECK(results.size() == 2);
        CHECK(results[0] == 15); // 10 + 5
        CHECK(results[1] == 20); // 10 * 2
        CHECK(call_count_1 == 1);
        CHECK(call_count_2 == 1);
    }

    SUBCASE("Removing listeners") {
        int call_count = 0;
        auto id = dispatcher.add_listener([&](int, double) {
            call_count++;
            return 0;
        });

        CHECK(dispatcher.remove_listener(id) == true);

        auto results = dispatcher.dispatch(1, 1.0);
        CHECK(results.empty());
        CHECK(call_count == 0);

        // Try removing non-existent ID
        CHECK(dispatcher.remove_listener(999) == false);
    }

    SUBCASE("Adding empty listener") {
        // Fix: Ensure the Listener type matches the dispatcher's Args... (int, double)
        moto::EventDispatcher<int, double>::Listener empty_l = nullptr;
        auto id = dispatcher.add_listener(empty_l);

        CHECK(id == static_cast<moto::EventDispatcher<int, double>::ListenerId>(-1));
    }
}

TEST_CASE("EventDispatcher Template Arguments") {
    moto::EventDispatcher<const std::string&, int> dispatcher;

    auto id = dispatcher.add_listener([](const std::string& s, int n) {
        return static_cast<int>(s.length()) + n;
    });

    auto results = dispatcher.dispatch("hello", 5);
    CHECK(results[0] == 10);
}

TEST_CASE("Helper functions add_event_listener") {
    moto::EventDispatcher<int> dispatcher;
    auto shared_dispatcher = std::make_shared<moto::EventDispatcher<int>>();

    bool called_direct = false;
    bool called_shared = false;

    // Test helper for reference
    moto::add_event_listener(dispatcher, [&](int) {
        called_direct = true;
        return 0;
    });

    // Test helper for shared_ptr
    moto::add_event_listener(shared_dispatcher, [&](int) {
        called_shared = true;
        return 0;
    });

    dispatcher.dispatch(1);
    shared_dispatcher->dispatch(1);

    CHECK(called_direct == true);
    CHECK(called_shared == true);
}

TEST_CASE("EventDispatcher Edge Cases") {
    moto::EventDispatcher<int> dispatcher;

    SUBCASE("Dispatch with no listeners") {
        auto results = dispatcher.dispatch(100);
        CHECK(results.empty());
    }

    SUBCASE("Multiple removals") {
        auto id1 = dispatcher.add_listener([](int x) { return x; });
        auto id2 = dispatcher.add_listener([](int x) { return x; });

        CHECK(dispatcher.remove_listener(id1) == true);
        CHECK(dispatcher.remove_listener(id1) == false); // Already removed
        CHECK(dispatcher.remove_listener(id2) == true);
    }
}
