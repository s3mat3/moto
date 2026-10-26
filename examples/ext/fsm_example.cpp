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


#include "fsm.hpp"
#include <iostream>
#include <memory>
#include <string>

// Event IDs
namespace Event {
    inline constexpr moto::event_id TICK_EVENT = 1;
}

// Context to hold the shared data of the traffic lights
struct TrafficContext {
    enum class Color { RED, GREEN, YELLOW };

    Color east_west = Color::RED;
    Color north_south = Color::RED;
    int timer = 0;

    void resetTimer() { timer = 0; }
    void tick() { timer++; }

    void printStatus() const {
        auto colorToString = [](Color c) {
            switch(c) {
                case Color::RED:    return "RED";
                case Color::GREEN:  return "GREEN";
                case Color::YELLOW: return "YELLOW";
                default:            return "UNKNOWN";
            }
        };
        std::cout << "[Time: " << timer << "] EW: " << colorToString(east_west)
                  << " | NS: " << colorToString(north_south) << std::endl;
    }
};

int main() {
    // 1. Create Context
    auto context = std::make_shared<TrafficContext>();

    // 2. Define States
    // IDs: 0: EW_GREEN, 1: EW_YELLOW, 2: NS_GREEN, 3: NS_YELLOW
    auto s_ew_green  = std::make_shared<moto::State<TrafficContext>>(0, "EW_GREEN");
    auto s_ew_yellow = std::make_shared<moto::State<TrafficContext>>(1, "EW_YELLOW");
    auto s_ns_green  = std::make_shared<moto::State<TrafficContext>>(2, "NS_GREEN");
    auto s_ns_yellow = std::make_shared<moto::State<TrafficContext>>(3, "NS_YELLOW");

    // Define behavior for each state
    s_ew_green->setEntry([](TrafficContext& c) {
        c.east_west = TrafficContext::Color::GREEN;
        c.north_south = TrafficContext::Color::RED;
        c.resetTimer();
    });

    s_ew_yellow->setEntry([](TrafficContext& c) {
        c.east_west = TrafficContext::Color::YELLOW;
        c.north_south = TrafficContext::Color::RED;
        c.resetTimer();
    });

    s_ns_green->setEntry([](TrafficContext& c) {
        c.east_west = TrafficContext::Color::RED;
        c.north_south = TrafficContext::Color::GREEN;
        c.resetTimer();
    });

    s_ns_yellow->setEntry([](TrafficContext& c) {
        c.east_west = TrafficContext::Color::RED;
        c.north_south = TrafficContext::Color::YELLOW;
        c.resetTimer();
    });

    // 3. Create FSM
    moto::Fsm<TrafficContext> fsm(context, s_ew_green);
    fsm.addState(s_ew_green);
    fsm.addState(s_ew_yellow);
    fsm.addState(s_ns_green);
    fsm.addState(s_ns_yellow);

    // 4. Define Transitions with Guards based on timing constraints
    // EW_GREEN (2) -> EW_YELLOW
    auto t1 = std::make_shared<moto::Transition<TrafficContext>>(0, Event::TICK_EVENT, s_ew_yellow);
    t1->setGuard([](TrafficContext& c) { return c.timer >= 2; });
    fsm.addTransition(t1);

    // EW_YELLOW (1) -> NS_GREEN
    auto t2 = std::make_shared<moto::Transition<TrafficContext>>(1, Event::TICK_EVENT, s_ns_green);
    t2->setGuard([](TrafficContext& c) { return c.timer >= 1; });
    fsm.addTransition(t2);

    // NS_GREEN (2) -> NS_YELLOW
    auto t3 = std::make_shared<moto::Transition<TrafficContext>>(2, Event::TICK_EVENT, s_ns_green); // Self-loop is handled by FSM, but we want to go to yellow
    auto t3_real = std::make_shared<moto::Transition<TrafficContext>>(2, Event::TICK_EVENT, s_ns_yellow);
    t3_real->setGuard([](TrafficContext& c) { return c.timer >= 2; });
    fsm.addTransition(t3_real);

    // NS_YELLOW (1) -> EW_GREEN
    auto t4 = std::make_shared<moto::Transition<TrafficContext>>(3, Event::TICK_EVENT, s_ew_green);
    t4->setGuard([](TrafficContext& c) { return c.timer >= 1; });
    fsm.addTransition(t4);

    // Initial trigger
    fsm.initial();

    // Simulation Loop
    std::cout << "Starting Traffic Light Simulation..." << std::endl;
    for (int i = 0; i < 20; ++i) {
        context->printStatus();
        context->tick();
        fsm.dispatch(Event::TICK_EVENT);
    }

    return 0;
}

