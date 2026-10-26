/**
 * @file event_dispatcher.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief A thread-safe event dispatcher for managing and notifying listeners.
 *
 * @author s3mat3
 */

#pragma once

#ifndef MOTO_EVENT_DISPATCHER_HPP
# define MOTO_EVENT_DISPATCHER_HPP

# include <algorithm>
# include <functional>
# include <memory>
# include <mutex>
# include <vector>

///@addtogroup core
///@{
/// @brief core event dispatcher and helpers
namespace moto {

/** A thread-safe dispatcher that allows registering listeners for specific
event arguments.
 * @tparam Args The argument types passed to the listeners when an event is dispatched.
 * @example core/event_dispatcher_example.cpp
 */
template <typename... Args>
class EventDispatcher final {
public:
    using Listener = std::function<int(
        Args...)>;  ///< Type definition for the listener callback function.
                    /// The callback must return an int and accept the template arguments Args...
    using ListenerId = std::size_t; ///< Unique identifier for a registered listener.

private:
    /** Internal structure to associate a listener with its unique ID.
     */
    struct ListenerEntry {
        ListenerId id;
        Listener callback;
    };
    std::vector<ListenerEntry> m_listeners;
    ListenerId m_next_id = 0;
    mutable std::mutex m_mutex;

public:
    /** Default constructor.
     */
    EventDispatcher() = default;

    /** Registers a new listener callback.
     *
     * @param[in] l The callback function to be executed during dispatch.
     * @retval The unique ListenerId of the added listener
     * @retval  static_cast<ListenerId>(-1) if the listener is null.
     */
    auto add_listener(Listener l) -> ListenerId {
        if (!l) return static_cast<ListenerId>(-1);

        std::lock_guard<std::mutex> lock(m_mutex);
        ListenerId id = m_next_id++;
        m_listeners.push_back({id, std::move(l)});
        return id;
    }

    /** Notifies all registered listeners with the provided arguments.
     *
     * @param[in] args The arguments to pass to each listener callback.
     * @return A vector containing the return values from each listener in the order they were registered.
     */
    auto dispatch(Args... args) const -> std::vector<int> {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<int> results;
        results.reserve(m_listeners.size());

        for (const auto& entry : m_listeners) {
            results.push_back(entry.callback(args...));
        }
        return results;
    }

    /** Removes a registered listener from the dispatcher.
     *
     * @param[in] id The unique ID of the listener to remove.
     * @retval True if the listener was successfully found and removed
     * @retval false otherwise.
     */
    auto remove_listener(ListenerId id) -> bool {
        std::lock_guard<std::mutex> lock(m_mutex); // Lock added
        auto it = std::remove_if(m_listeners.begin(), m_listeners.end(),
                                 [id](const auto& entry) { return entry.id == id; });
        if (it != m_listeners.end()) {
            m_listeners.erase(it, m_listeners.end());
            return true;
        }
        return false;
    }
};  //<-- class EventDispatcher ends here.
    ///

/** Helper function to add a listener to an EventDispatcher instance.
 *
 * @tparam Args The argument types of the dispatcher.
 * @tparam F The type of the callable object.
 * @param[in] e Reference to the EventDispatcher.
 * @param[in] l The callable listener.
 * @return The unique ListenerId of the added listener.
 */
template <typename... Args, typename F>
inline auto add_event_listener(EventDispatcher<Args...>& e, F&& l) {
    return e.add_listener(std::forward<F>(l));
}

/** Helper function to add a listener to an EventDispatcher managed by a shared_ptr.
 *
 * @tparam Args The argument types of the dispatcher.
 * @tparam F The type of the callable object.
 * @param[in] e Shared pointer to the EventDispatcher.
 * @param[in] l The callable listener.
 * @return The unique ListenerId of the added listener.
 */
template <typename... Args, typename F>
inline auto add_event_listener(const std::shared_ptr<EventDispatcher<Args...>>& e, F&& l) {
    return e->add_listener(std::forward<F>(l));
}

} //<-- namespace moto ends here.
///@}
#endif //<-- MOTO_EVENT_DISPATCHER_HPP ends here.
