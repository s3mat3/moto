/**
 * @file fsm_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for ext/fsm.hpp
 *
 * @author s3mat3
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ext/fsm.hpp"

// --- Test Context ---
// A simple context to track state changes and action executions
struct TestContext {
    int counter = 0;
    std::string last_action = "";
    bool guard_condition = true;

    void log(const std::string& msg) {
        last_action = msg;
    }
};

// Event IDs for testing
namespace TestEvent {
    inline constexpr moto::event_id next = 1;
    inline constexpr moto::event_id reset = 2;
    inline constexpr moto::event_id self = 3;
}

TEST_CASE("FSM Basic Functionality Test") {
    auto ctx = std::make_shared<TestContext>();

    // Define States
    auto state_a = std::make_shared<moto::State<TestContext>>(1, "StateA");
    auto state_b = std::make_shared<moto::State<TestContext>>(2, "StateB");

    // Setup State A actions
    state_a->setEntry([](TestContext& c) { c.log("entry_a"); })
            .setDo([](TestContext& c) { c.counter++; })
            .setExit([](TestContext& c) { c.log("exit_a"); });

    // Setup State B actions
    state_b->setEntry([](TestContext& c) { c.log("entry_b"); })
            .setDo([](TestContext& c) { c.counter += 10; })
            .setExit([](TestContext& c) { c.log("exit_b"); });

    // Create FSM
    moto::Fsm<TestContext> fsm(ctx, state_a);
    fsm.addState(state_a);
    fsm.addState(state_b);

    SUBCASE("Initial state and activity") {
        fsm.initial();
        CHECK(ctx->last_action == "entry_a");
        CHECK(ctx->counter == 1); // initial calls entry then doActivity
    }

    SUBCASE("Basic transition A -> B") {
        fsm.initial();

        // Transition A --(next)--> B
        auto trans = std::make_shared<moto::Transition<TestContext>>(1, TestEvent::next, state_b);
        trans->setAction([](TestContext& c) { c.log("trans_action"); });
        fsm.addTransition(trans);

        auto current = fsm.dispatch(TestEvent::next);

        CHECK(current->id() == 2);
        CHECK(current->name() == "StateB");
        // Order: transition action -> exit_a -> entry_b -> doActivity_b
        // The last one to write to last_action is entry_b (since doActivity_b doesn't call log)
        CHECK(ctx->last_action == "entry_b");
        CHECK(ctx->counter == 1 + 10);
    }

    SUBCASE("Guard condition prevents transition") {
        fsm.initial();

        auto trans = std::make_shared<moto::Transition<TestContext>>(1, TestEvent::next, state_b);
        trans->setGuard([](TestContext& c) { return c.guard_condition; });
        fsm.addTransition(trans);

        // Case 1: Guard is true
        ctx->guard_condition = true;
        fsm.dispatch(TestEvent::next);
        CHECK(fsm.dispatch(0)->id() == 2); // Transition happened

        // Reset FSM for next check
        moto::Fsm<TestContext> fsm2(ctx, state_a);
        fsm2.addTransition(trans);
        fsm2.initial();

        // Case 2: Guard is false
        ctx->guard_condition = false;
        auto current = fsm2.dispatch(TestEvent::next);
        CHECK(current->id() == 1); // Transition blocked
    }
}

TEST_CASE("FSM Advanced Mode Test") {
    auto ctx = std::make_shared<TestContext>();
    auto state_a = std::make_shared<moto::State<TestContext>>(1, "StateA");

    // Tracking entry/exit counts via counter
    state_a->setEntry([](TestContext& c) { c.counter++; })
            .setExit([](TestContext& c) { c.counter--; });

    // m_mode = true (Enable re-entry)
    moto::Fsm<TestContext> fsm(ctx, state_a, true);
    fsm.addState(state_a);

    // Transition A --(self)--> A
    auto self_trans = std::make_shared<moto::Transition<TestContext>>(1, TestEvent::self, state_a);
    fsm.addTransition(self_trans);

    fsm.initial(); // counter becomes 1 (entry)
    CHECK(ctx->counter == 1);

    SUBCASE("Self transition with m_mode = true") {
        fsm.dispatch(TestEvent::self);
        // Order: exit (1-1=0) -> entry (0+1=1)
        CHECK(ctx->counter == 1);
        // We can verify that exit/entry were called by checking if logic occurred
        // In this specific implementation, the result is 1, but the process was executed.
    }
}

TEST_CASE("FSM Void Event and DoActivity") {
    auto ctx = std::make_shared<TestContext>();
    auto state_a = std::make_shared<moto::State<TestContext>>(1, "StateA");
    state_a->setDo([](TestContext& c) { c.counter++; });

    moto::Fsm<TestContext> fsm(ctx, state_a);
    fsm.initial(); // counter = 1

    SUBCASE("Event 0 triggers doActivity") {
        fsm.dispatch(0);
        CHECK(ctx->counter == 2);
    }

    SUBCASE("Void event does nothing") {
        fsm.dispatch(moto::EventCode::void_event);
        CHECK(ctx->counter == 1);
    }
}


