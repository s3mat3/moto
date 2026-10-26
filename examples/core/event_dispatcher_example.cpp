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
#include <memory>
#include <vector>

#include "event_dispatcher.hpp"

// --- Event Argument ---
// Data structures delivered with events
struct LoginEvent {
    std::string username;
    int user_id;
};

// --- Listener Class A: Logging System ---
class Logger {
public:
    int on_login(const LoginEvent& event) {
        std::cout << "[Logger] User logged in: " << event.username
                  << " (ID: " << event.user_id << ")" << std::endl;
        return 0; // Success
    }
};

// --- Listener Class B: Session Manager ---
class SessionManager {
public:
    int on_login(const LoginEvent& event) {
        std::cout << "[SessionManager] Creating session for " << event.username << std::endl;
        // Example of returning an error under some conditions
        if (event.user_id < 0) return -1;
        return 0;
    }
};

int main() {
    // 1. Create Dispatcher
    // Configure settings to deliver LoginEvent references (const LoginEvent&
    // to reduce copy costs)
    moto::EventDispatcher<const LoginEvent&> login_dispatcher;

    // 2. Generating a listener
    auto logger = std::make_shared<Logger>();
    auto session_mgr = std::make_shared<SessionManager>();

    // 3. Registering a listener
    // C++20 uses lambda capture to safely bind member functions
    // By capturing with shared_ptr you can guarantee the object's survival
    // while the Dispatcher is alive
    auto id1 = login_dispatcher.add_listener([logger](const LoginEvent& e) {
        return logger->on_login(e);
    });

    auto id2 = login_dispatcher.add_listener([session_mgr](const LoginEvent& e) {
        return session_mgr->on_login(e);
    });
    std::cout << "id2 : " << id2 << std::endl;
    // 4. Event occurrence (Dispatch)
    LoginEvent event1{"Alice", 1001};
    std::cout << "--- Dispatching Event 1 ---" << std::endl;
    std::vector<int> results1 = login_dispatcher.dispatch(event1);
    std::cout << "Results: " << results1.size() << " listeners responded." << std::endl;

    // 5. Release listener
    // Try unsubscribing from Logger
    std::cout << "\n--- Removing Logger Listener ---" << std::endl;
    login_dispatcher.remove_listener(id1);

    // 6. Event occurrence again
    LoginEvent event2{"Bob", 1002};
    std::cout << "--- Dispatching Event 2 ---" << std::endl;
    std::vector<int> results2 = login_dispatcher.dispatch(event2);
    std::cout << "Results: " << results2.size() << " listeners responded." << std::endl;

    // 7. Validating error cases
    LoginEvent event3{"InvalidUser", -1};
    std::cout << "\n--- Dispatching Event 3 (Error Case) ---" << std::endl;
    auto results3 = login_dispatcher.dispatch(event3);
    for (int r : results3) {
        if (r != 0) std::cout << "A listener reported an error: " << r << std::endl;
    }

    return 0;
}
