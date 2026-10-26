/**
 * @file timer.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief A thread-safe timer class for measuring elapsed time and handling timeouts.
 *
 * @author s3mat3
 */

#pragma once

#ifndef MOTO_TIMER_HPP
# define  MOTO_TIMER_HPP

# include <atomic>
# include "internal/common.hpp"
/// @addtogroup core
/// @{
namespace moto {

/** A high-resolution timer utilizing std::chrono and atomic operations.
 *
 * This class provides functionality to start, stop, suspend, and resume a
 * timer,
 * as well as check for timeouts with support for repeatable intervals.
 * @example core/timer_example.cpp
 */
class Timer final {
public:
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::nanoseconds;

    /** Construct a new Timer object.
     *
     * Initializes the timer in a stopped state.
     */
    Timer() : m_running(false) {}

    /** Start timer.
     *
     * Captures the current time as the starting point and sets the state to
     * running.
     */
    void start() noexcept {
        m_capture.store(clock::now(), std::memory_order_release);
        m_running.store(true, std::memory_order_release);
    }

    /** Stop timer.
     *
     * Sets the state to not running and calculates the time elapsed since the
     * last capture.
     * @return Elapsed time in nanoseconds.
     */
    duration stop() noexcept {
        m_running.store(false, std::memory_order_release);
        return clock::now() - m_capture.load(std::memory_order_acquire);
    }

    /** Suspend the timer.
     *
     * Sets the state to not running without resetting the capture point.
     */
    void suspend() noexcept { m_running.store(false, std::memory_order_release); }

    /** Resume the timer.
     *
     * Sets the state to running to continue timing from the last capture
     * point.
     */
    void resume()  noexcept { m_running.store(true, std::memory_order_release); }

    /** Check if the specified timeout has occurred.
     *
     * By receiving a duration type, you can directly pass literals such as
     * 100ms, 5s, etc.
     * Can use chrono literals since C++14.
     * @code
     * using namespace std::chrono_literals;
     * auto x = 100ms;
     * auto y = 5s;
     *    ... etc.
     * @endcode
     *
     * @tparam Rep The representation type of the duration.
     * @tparam Period The tick period of the duration.
     * @param[in] timeout The timeout duration to check against.
     * @param[in] repeatable If true, the timer resets to the next interval
     * upon timeout; if false, the timer stops (one-shot).
     * @retval true if the timeout occurred.
     * @retval false if the timeout has not been reached or the timer is not
     * running.
    */
    template <typename Rep, typename Period>
    bool isTimeout(std::chrono::duration<Rep, Period> timeout, bool repeatable = true) noexcept {
        if (!m_running.load(std::memory_order_acquire)) return false;

        auto now = clock::now();
        auto target_duration = std::chrono::duration_cast<duration>(timeout);
        time_point current_capture = m_capture.load(std::memory_order_acquire);
        if (now - m_capture.load(std::memory_order_acquire) >= target_duration) {
            if (!repeatable) {
                m_running.store(false, std::memory_order_release);
                MOTO_LOG("Done one shot timer!!");
            } else {
                // Add atomically in CAS loop instead of fetch_add
                time_point next_capture = current_capture + target_duration;
                while (!m_capture.compare_exchange_weak(
                    current_capture, next_capture, std::memory_order_acq_rel,
                    std::memory_order_acquire)) {
                    // If it fails, current_capture is updated to the latest
                    // value and the next calculation is restarted.
                    next_capture = current_capture + target_duration;
                }
            }
            return true;
        }
        return false;
    }

    /** Check if the timer is currently running.
     * @retval true if running.
     * @retval false otherwise.
     */
    explicit operator bool() const noexcept {
        return static_cast<bool>(m_running.load(std::memory_order_acquire));
    }

private:
    std::atomic<bool>       m_running; ///< Atomic flag indicating whether the timer is active.
    std::atomic<time_point> m_capture; ///< Atomic timestamp of the last start or interval reset.
}; //<-- class Timer ends here.


} //<-- namespace moto ends here.
///@}
#endif //<-- macro  MOTO_TIMER_HPP ends here.
