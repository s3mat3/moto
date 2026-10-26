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
#include <string>

#include "event_bus.hpp"

// Dummy structure
struct DamageInfo {
    int amount;
    std::string attacker;
};

int main() {
    // 1. Get an instance of EventBus as singleton
    auto& bus = moto::EventBus::instance();

    // Case A: Event of simple type (int, std::string)
    auto log_id = bus.add_listener<int, std::string>([](int level, std::string message) {
        std::cout << "[Log System] Level: " << level << " Message: " << message << std::endl;
        return 0;
    });

    std::cout << "--- Dispatching Log Event ---" << std::endl;
    bus.dispatch<int, std::string>(1, "System started successfully.");
    bus.dispatch<int, std::string>(3, "Warning: High memory usage detected.");


    // Case B: Event using custom structure (DamageInfo)
    // Registering a listener (e.g. UI update processing, etc.)
    bus.add_listener<DamageInfo>([](DamageInfo info) {
        std::cout << "[UI System] Displaying damage: " << info.amount 
                  << " from " << info.attacker << std::endl;
        return 1;
    });

    // Registering listeners (e.g. sound effect playback processing, etc.)
    bus.add_listener<DamageInfo>([](DamageInfo info) {
        std::cout << "[Audio System] Playing hit sound for " << info.amount << " damage." << std::endl;
        return 1;
    });

    std::cout << "\n--- Dispatching Damage Event ---" << std::endl;
    bus.dispatch<DamageInfo>({50, "Goblin"});


    // Remove listenner
    std::cout << "\n--- Removing Log Listener and Dispatching again ---" << std::endl;
    if (bus.remove_listener<int, std::string>(log_id)) {
        std::cout << "Successfully removed log listener." << std::endl;
    }

    // dispatch here won't output anything because I removed it
    bus.dispatch<int, std::string>(1, "This message will not be printed.");

    return 0;
}

