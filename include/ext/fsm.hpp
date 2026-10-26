/**
 * @file fsm.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief Finite State Machine (FSM) implementation.
 *
 * @author s3mat3
 *
 * @example ext/fsm_example.cpp
 */

#pragma once

#ifndef FSM_Hpp
# define  FSM_Hpp

# include <string>
# include <vector>
# include <unordered_map>
# include <memory>
# include <functional>

# include "../core/internal/common.hpp"
/// @addtogroup ext
/// @{
namespace moto {

/** Identifier type for events. */
using event_id = int;

/** Collection of predefined event codes. */
namespace EventCode {
/** Special event ID used to indicate no event or a null event. */
inline constexpr event_id void_event = -1;
}

/** State class
 *
 * Defines the behavioral logic ("what to do") for a specific state within the
 * FSM.
 * @tparam C The context class that holds the state data and is operated on by
 * actions.
 */
template <class C>
class State {
public:
    /** Functional type for actions executed within the state. */
    using Action = std::function<void(C&)>;

private:
    int         m_id      {0};         ///< Unique identifier for the state.
    std::string m_name    {"No name"}; ///< Human-readable name for debugging.
    Action      m_entry_act {nullptr}; ///< Action executed when entering the state.
    Action      m_do_act    {nullptr}; ///< Action executed during the state's active period.
    Action      m_exit_act  {nullptr}; ///< Action executed when exiting the state.

public:
    /** Constructor for State.
     * @param id Unique identifier.
     * @param name Descriptive name of the state.
     */
    State(int id, const std::string& name) : m_id{id}, m_name{name} {}

    /** Sets the entry action.
     * @param act The action to execute on entry.
     * @return Reference to this State for chaining.
     */
    auto setEntry(Action act) -> State& { m_entry_act = std::move(act); return *this; }

    /** Sets the do action.
     * @param act The action to execute periodically while in this state.
     * @return Reference to this State for chaining.
     */
    auto setDo(Action act)    -> State& { m_do_act = std::move(act);    return *this; }

    /** Sets the exit action.
     * @param act The action to execute on exit.
     * @return Reference to this State for chaining.
     */
    auto setExit(Action act)  -> State& { m_exit_act = std::move(act);  return *this; }

    /** Executes the entry action.
     * @param context The context object.
     */
    auto entry(C& context) noexcept -> void { if(m_entry_act) m_entry_act(context); }

    /** Executes the main activity action.
     * @param context The context object.
     */
    auto doActivity(C& context) noexcept -> void { if(m_do_act) m_do_act(context); }

    /** Executes the exit action.
     * @param context The context object.
     */
    auto exit(C& context) noexcept -> void { if(m_exit_act) m_exit_act(context); }

    /** Returns the state ID. */
    auto id() const noexcept -> int { return m_id; }

    /** Returns the state name. */
    auto name() const noexcept -> std::string { return m_name; }
}; //<-- class State ends here.

/** Transition class
 *
 * Defines the rules for moving from one state to another based on events and
 * guard conditions.
 * @tparam C The context class.
 */
template <class C>
class Transition {
public:
    /** Functional type for guard conditions (must return true to allow transition). */
    using Guard = std::function<bool(C&)>;
    /** Functional type for actions executed during the transition. */
    using Action = std::function<void(C&)>;
    /** Shared pointer to a State object. */
    using state_ptr = std::shared_ptr<State<C>>;

private:
    int         m_from_id;           ///< ID of the source state.
    event_id    m_event;             ///< Event ID that triggers this transition.
    state_ptr   m_to_state;          ///< Destination state.
    Guard       m_guard   {nullptr}; ///< Condition that must be met for transition.
    Action      m_action  {nullptr}; ///< Action to perform during transition.

public:
    /** Constructor for Transition.
     * @param from Source state ID.
     * @param e Triggering event ID.
     * @param to Destination state.
     */
    Transition(int from, event_id e, state_ptr to)
        : m_from_id{from}, m_event{e}, m_to_state{std::move(to)} {}

    /** Sets the guard condition.
     * @param g The guard function.
     * @return Reference to this Transition for chaining.
     */
    auto setGuard(Guard g) -> Transition& { m_guard = std::move(g); return *this; }

    /** Sets the transition action.
     * @param a The action function.
     * @return Reference to this Transition for chaining.
     */
    auto setAction(Action a) -> Transition& { m_action = std::move(a); return *this; }

    // Determine if this transition is applicable and return the next state
    // if possible

    /** Evaluates if the transition can occur.
     * @param context The context object.
     * @return Pointer to the destination state if conditions are met,
     * otherwise nullptr.
     */
    auto evaluate(C& context) const -> state_ptr {
        if (m_guard && !m_guard(context)) {
            return nullptr; // Does not meet guard conditions
        }
        if (m_action) m_action(context); // Execute action on transition
        return m_to_state;
    }

    /** Returns the source state ID. */
    auto from_id() const noexcept -> int { return m_from_id; }
    /** Returns the trigger event ID. */
    auto get_event_id() const noexcept -> event_id { return m_event; }
}; //<-- class Transition ends here.

/** Fsm class
 *
 * Manages states and transitions, and handles event dispatching to drive the
 * state machine.
 * @tparam C The context class.
 */
template <class C>
class Fsm {
public:
    using state_ptr = std::shared_ptr<State<C>>;
    using trans_ptr = std::shared_ptr<Transition<C>>;

private:
    std::shared_ptr<C> m_context {nullptr}; ///< Shared context used across all states and transitions.
    state_ptr          m_current {nullptr}; ///< The current active state.
    state_ptr          m_prev    {nullptr}; ///< The previously active state.
    bool               m_mode    {false};   ///< Flag to enable re-entry of the same state.
    /** Internal storage to ensure all state objects are kept in memory. */
    std::vector<state_ptr> m_all_states;
    /** Table mapping "from_id_event_id" keys to transition objects. */
    std::unordered_map<std::string, trans_ptr> m_transitions;

    /** Generates a unique key for the transition map.
     * @param from Source state ID.
     * @param e Event ID.
     * @return A string key in the format "id_event".
     */
    auto make_key(int from, event_id e) const -> std::string {
        return std::to_string(from) + "_" + std::to_string(e);
    }

public:
    /** Constructor for Fsm.
     * @param context Shared context for the FSM.
     * @param initial The initial starting state.
     * @param mode Whether to allow state re-entry (defaults to false).
     */
    Fsm(std::shared_ptr<C> context, state_ptr initial, bool mode = false)
        : m_context(context), m_current(initial), m_mode(mode) {}

    /** Registers a state with the FSM.
     * @param s Pointer to the state to add.
     */
    auto addState(state_ptr s) -> void {
        if (s) m_all_states.push_back(s);
    }

    /** Registers a transition rule with the FSM.
     * @param t Pointer to the transition to add.
     */
    auto addTransition(trans_ptr t) -> void {
        if (t) m_transitions[make_key(t->from_id(), t->get_event_id())] = t;
    }

    /** Initializes the FSM by triggering entry and activity of the initial
     * state.
     */
    auto initial() noexcept {
        if (!m_current || !m_context) return;
        m_current->entry(*m_context);
        m_current->doActivity(*m_context);
    }

    /** Dispatches an event to trigger potential state transitions.
     * @param e The event ID to process.
     * @return The state the FSM is in after processing the event.
     */
    virtual auto dispatch(event_id e) noexcept -> state_ptr {
        if (!m_current || !m_context || e == EventCode::void_event) return m_current;

        if (e == 0) {
            m_current->doActivity(*m_context);
        } else {
            // Search for transition rules
            auto it = m_transitions.find(make_key(m_current->id(), e));
            if (it != m_transitions.end()) {
                auto next = it->second->evaluate(*m_context);
                if (next && next != m_current) {
                    // transition
                    m_current->exit(*m_context);
                    m_prev = m_current;
                    m_current = next;

# if defined (MOTO_DEBUG_FSM)
                    MOTO_LOG("**Change state** from " + m_prev->name() + " to " + m_current->name());
# endif
                    m_current->entry(*m_context);
                    m_current->doActivity(*m_context);
                } else if (m_mode && next == m_current) {
                    // reenter/exit
                    m_current->exit(*m_context);
                    m_current->entry(*m_context);
                    m_current->doActivity(*m_context);
                }
            }
        }
        return m_current;
    }
}; //<-- class Fsm ends here.


} //<-- namespace moto ends here.
/// @}
#endif //<-- macro  FSM_Hpp ends here.
