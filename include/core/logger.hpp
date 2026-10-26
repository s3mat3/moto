/**
 * @file logger.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief Logging utility providing leveled output with source location tracking.
 *
 * @author s3mat3
 */

#pragma once

#ifndef MOTO_LOGGER_HPP
# define  MOTO_LOGGER_HPP


# include <array>
# include "internal/common.hpp"

///@addtogroup core
///@{
namespace moto {
/** Log levels for filtering and categorization.
 */
enum class LOG_LEVEL : int {
    V = 0, ///< Log void
    F,     ///< Fatal error
    E,     ///< Error
    W,     ///< Warning
    N,     ///< Notice
    I,     ///< Info
    D,     ///< Debug
};

/** Convert an integer index to a LOG_LEVEL.
 * @param[in] l The integer index.
 * @return The corresponding LOG_LEVEL.
 */
inline constexpr auto index2LogLevel(int l) -> LOG_LEVEL {
  switch (l) {
  case 0: return LOG_LEVEL::V;
  case 1: return LOG_LEVEL::F;
  case 2: return LOG_LEVEL::E;
  case 3: return LOG_LEVEL::W;
  case 4: return LOG_LEVEL::N;
  case 5: return LOG_LEVEL::I;
  case 6:
  default: return LOG_LEVEL::D;
  }
}

/** Convert a LOG_LEVEL to its integer index.
 * @param[in] l The log level.
 * @return The integer value of the log level.
 */
inline constexpr auto LogLevel2index(LOG_LEVEL l) -> int {
    return static_cast<int>(l);
}

/** String representations of each log level for display.
 */
static constexpr std::array<std::string_view, 7> LogLevelNameList = {
    "VOID  ",
    "Fatal ",
    "Error ",
    "Warn  ",
    "Notice",
    "Info  ",
    "Debug ",
}; //<-- LogLevelNameList ends here.

/** Determine the ANSI color code based on the log level.
 * @param[in] level The log level.
 * @return The associated color string_view.
 */
inline constexpr auto get_color_for_level(LOG_LEVEL level) -> std::string_view {
    switch (level) {
    case LOG_LEVEL::F:
    case LOG_LEVEL::E: return Color::RED;
    case LOG_LEVEL::W: return Color::YELLOW;
    case LOG_LEVEL::N: return Color::GREEN;
    case LOG_LEVEL::I: return Color::BLUE;
    default:           return Color::NONE;
    }
}

/** Extract the function name from source_location if in debug mode.
 *
 * If the log level is debug, extract the function name from source_location.
 * @param[in] l Current log level.
 * @param[in] si Source location information.
 * @return Function name if level is DEBUG, otherwise an empty string.
 */
inline constexpr auto extract_func_name_neadle(LOG_LEVEL l, const src_info si) -> const char* {
    if (l != LOG_LEVEL::D) {
        return "";
    }
    return si.function_name();
}


/** Configuration settings for the Logger.
 */
class LogConfig final {
    LOG_LEVEL m_level {LOG_LEVEL::D}; ///< Current minimum log level filter.
    Color::Config m_cconf{};          ///< Color output configuration.

public:
    /** Default constructor.
     * Initializes level to Debug and auto-detects terminal.
     */
    LogConfig() : m_level(LOG_LEVEL::D), m_cconf() {
        m_cconf.auto_detect_terminal();
    }

    /** Set whether color output is enabled.
     * @param[in] c True to enable color, false to disable.
     */
    auto use_color(bool c) noexcept -> void {m_cconf.use_color(c);}

    /** Check if color output is currently enabled.
     * @return True if enabled, false otherwise.
     */
    auto use_coror() const noexcept -> bool {return m_cconf.use_color();}

    /** Set the minimum log level.
     * @param[in] l The new log level filter.
     */
    auto level(LOG_LEVEL l) noexcept { m_level = l; }

    /** Get the current minimum log level.
     * @return The current LOG_LEVEL.
     */
    auto level() const noexcept { return m_level; }
};

/** Singleton logger class for handling application-wide logging.
 * @example core/logger_example.cpp
 */
class Logger final {
    LogConfig m_lc{}; ///< Configuration instance.

    /** Default constructor.
     */
    Logger() : m_lc() {}

public:
    /** Get the singleton instance of the Logger.
     * @return Reference to the Logger instance.
     */
    static auto instance() -> Logger& {
        static Logger logger;
        return logger;
    }

    /** Update the global log level.
     * @param[in] l The new log level.
     */
    auto level(LOG_LEVEL l) noexcept { m_lc.level(l); }

    /** Update the global color output setting.
     * @param[in] c True to enable color, false to disable.
     */
    auto use_color(bool c) noexcept {m_lc.use_color(c);}

    /** Log a message with the specified level and source information.
     * @param[in] l Log level of the message.
     * @param[in] m The message string.
     * @param[in] si Source location information (defaults to current
     * location).
     */
    auto log(LOG_LEVEL l, std::string_view m,
             const src_info si = src_info::current()) {

        if (m_lc.level() < l) return;

        auto now = getCurrentClockCount();
        auto tid = std::format("{}", std::this_thread::get_id());
        auto level_name = LogLevelNameList[static_cast<int>(l)];

        auto fname = removePath(si.file_name());
        auto fun = extract_func_name_neadle(m_lc.level(), si);
        auto cstart = m_lc.use_coror() ? get_color_for_level(l) : Color::NONE;
        auto cend   = m_lc.use_coror() && cstart != Color::NONE ? Color::RESET : Color::NONE;
        internal::write_clog(m, now, level_name, fname, si.line(), fun, cstart, cend, tid);
    }
};

} //<-- namespace moto ends here.

# if defined (MOTO_USE_LOG) || defined (DEBUG)
#  define LOG_F(msg) do{moto::Logger::instance().log(moto::LOG_LEVEL::F, msg);} while(0)
#  define LOG_E(msg) do{moto::Logger::instance().log(moto::LOG_LEVEL::E, msg);} while(0)
#  define LOG_W(msg) do{moto::Logger::instance().log(moto::LOG_LEVEL::W, msg);} while(0)
#  define LOG_N(msg) do{moto::Logger::instance().log(moto::LOG_LEVEL::N, msg);} while(0)
#  define LOG_I(msg) do{moto::Logger::instance().log(moto::LOG_LEVEL::I, msg);} while(0)
#  define LOG_D(msg) do{moto::Logger::instance().log(moto::LOG_LEVEL::D, msg);} while(0)
# else
#  define LOG_F(msg)
#  define LOG_E(msg)
#  define LOG_W(msg)
#  define LOG_N(msg)
#  define LOG_I(msg)
#  define LOG_D(msg)
# endif
///@}
#endif //<-- macro  LOGGER_Hpp ends here.
