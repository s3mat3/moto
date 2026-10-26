/**
 * @file event_bus.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief Event bus implementation for decoupled communication.
 *
 * @author s3mat3
 */

#pragma once

#ifndef MOTO_EVENT_BUS_HPP
# define MOTO_EVENT_BUS_HPP

# include <any>
# include <typeindex>
# include <unordered_map>

# include "../core/event_dispatcher.hpp"
///@addtogroup ext
///@{
namespace moto {

/** EventBus provides a centralized event management system.
 *
 * It allows different parts of the application to communicate without
 * having direct dependencies on each other.
 * @example ext/event_bus_example.cpp
 */
class EventBus final {
public:
    /** Prevent copying of the singleton instance. */
    EventBus(const EventBus&) = delete;
    /** Prevent assignment of the singleton instance. */
    EventBus& operator=(const EventBus&) = delete;

    /** Get the singleton instance of EventBus.
     * @return Reference to the global EventBus instance.
     */
    static EventBus& instance() {
        static EventBus instance;
        return instance;
    }

    /** Registers a listener for a specific event type.
     * @tparam Args The argument types of the event.
     * @tparam F The callable type (lambda, function pointer, etc.)
     * @param listener The callback function to be executed when the event is
     * dispatched.
     * @return The unique ID of the registered listener, or
     * static_cast<std::size_t>(-1) on failure.
     */
    template <typename... Args, typename F>
    auto add_listener(F&& listener) -> std::size_t {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto dispatcher = get_or_create_dispatcher<Args...>();
        if (!dispatcher) return static_cast<std::size_t>(-1);

        // Explicitly wrap the callable into std::function
        using ListenerType = typename EventDispatcher<Args...>::Listener;
        return dispatcher->add_listener(ListenerType(std::forward<F>(listener)));
    }

    /** Dispatches an event to all registered listeners of the corresponding type.
     * @tparam Args The argument types of the event.
     * @param args The arguments to pass to the listeners.
     * @return A vector of return values from each listener.
     */
    template <typename... Args>
    auto dispatch(Args... args) -> std::vector<int> {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto dispatcher = get_or_create_dispatcher<Args...>();
        if (!dispatcher) return {};

        return dispatcher->dispatch(std::forward<Args>(args)...);
    }
    /** Removes a listener from a specific event type.
     * @tparam Args The argument types of the event.
     * @param id The ID of the listener to remove.
     * @return True if the listener was successfully removed, false otherwise.
     */
    template <typename... Args>
    auto remove_listener(std::size_t id) -> bool {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto dispatcher = get_or_create_dispatcher<Args...>();
        if (!dispatcher) return false;

        return dispatcher->remove_listener(id);
    }

    /** Clears all registered dispatchers.
     *
     * Essential for resetting state between tests.
     */
    void clear_all() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dispatchers.clear();
    }

private:
    EventBus() = default;

    /** Retrieves an existing dispatcher for the given types or creates a new
     * one.
     * @tparam Args The argument types defining the dispatcher type.
     * @return A shared pointer to the EventDispatcher for the specified types.
     */
    template <typename... Args>
    auto get_or_create_dispatcher() -> std::shared_ptr<EventDispatcher<Args...>> {
        std::type_index type_idx = typeid(EventDispatcher<Args...>);

        auto it = m_dispatchers.find(type_idx);
        if (it != m_dispatchers.end()) {
            // Use pointer-based any_cast to avoid throwing bad_any_cast exception
            auto ptr = std::any_cast<std::shared_ptr<EventDispatcher<Args...>>>(&it->second);
            if (ptr) return *ptr;
        }

        auto dispatcher = std::make_shared<EventDispatcher<Args...>>();
        m_dispatchers[type_idx] = dispatcher;
        return dispatcher;
    }

    /** Map storing dispatchers indexed by their type index. */
    std::unordered_map<std::type_index, std::any> m_dispatchers;
    /** Mutex to ensure thread-safe access to the dispatchers map. */
    std::mutex m_mutex;

}; //<-- class EventBus ends here.

} //<-- namespace moto ends here.
///@}
#endif // MOTO_EVENT_BUS_HPP
