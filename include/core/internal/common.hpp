/**
 * @file common.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief Define global use constants and functions
 *
 * @author s3mat3
 */

#pragma once

#include <cstdint>
#ifndef MOTO_COMMON_HPP
# define  MOTO_COMMON_HPP

# include <chrono>
# include <format>
# include <iostream>
# include <source_location>
# include <string_view>
# include <syncstream>
# include <thread>
# include <unistd.h>

///@addtogroup internal
///@{

// Since c++23, formatter supports thread::id, but before that it was not supported, so add formatter.
# if __cplusplus < 202302L
template <>
struct std::formatter<std::thread::id> : std::formatter<std::string> {
    auto format(std::thread::id id, format_context& ctx) const {
        std::ostringstream oss;
        oss << id;
        return std::formatter<std::string>::format(oss.str(), ctx);
    }
};
# endif

namespace moto {
/////////////////
/// constants ///
/////////////////
/** ANSI terminal colors.
 */
namespace Color {
constexpr std::string_view RESET   = "\033[0m";
constexpr std::string_view BLACK   = "\033[30m";
constexpr std::string_view RED     = "\033[31m";
constexpr std::string_view GREEN   = "\033[32m";
constexpr std::string_view YELLOW  = "\033[33m";
constexpr std::string_view BLUE    = "\033[34m";
constexpr std::string_view MAGENTA = "\033[35m";
constexpr std::string_view CYAN    = "\033[36m";
constexpr std::string_view WHITE   = "\033[37m";
constexpr std::string_view NONE = "";
/** ANSI color config.
 */
struct Config {
    bool m_use_color{true};
    auto use_color() const noexcept -> bool {return m_use_color;}
    auto use_color(bool u) noexcept -> void {m_use_color = u;}
    auto auto_detect_terminal() noexcept {
        return m_use_color = isatty(fileno(stderr));
    }
}; //<-- struct Config ends here.

} //<--  namespace Color ends here.

/////////////////////////
/// moto global types ///
/////////////////////////
using src_info = std::source_location;

struct ColorConfig {
    bool use = true;
    auto use_color() const noexcept { return use; }
    auto auto_detect_terminal() noexcept {
        return use = isatty(fileno(stderr));
    }
};

///////////////////////////
/// functions and class ///
///////////////////////////
/** Remove path from full path
 *
 */
inline constexpr auto removePath(std::string_view target) -> std::string_view {
    // for Windows (\) and POSIX (/)
    const size_t pos = target.find_last_of("/\\");
    if (pos == std::string_view::npos) {
        return target; // If the path is not included, return it as is.
    }
    return target.substr(pos + 1);
} //<-- function removePath  ends here.

inline auto getCurrentTime() -> std::string {
    const auto now = std::chrono::system_clock::now();
    // Round down to millisecond precision
    const auto now_usec = std::chrono::floor<std::chrono::milliseconds>(now);
    return std::format("{:%FT%T}", now_usec);
}

inline auto getCurrentClockCount() -> std::string {
    return std::format("{: 23}", std::chrono::steady_clock::now().time_since_epoch().count());
}


namespace internal {
///                                    "139791231317888"     [Fatal ]:
constexpr std::string_view DUMMY_TID = "---------------";//  [MOTO_D]:
/** Write message to std::clog with many infos.
 *
 * output format
 * @code
 * 0010215186048523 [tid]  [tag]: msg fname:line:fun
 * @endcode
 *  @param[in] msg message for your given
 *  @param[in] now now count or date-time (formated string)
 *  @param[in] tag message tag default "MOTO_D"
 *  @param[in] fname File name where this message occurred default ""
 *  @param[in] line Line number where this message occurred default 0
 *  @param[in] fun Function name where this message occurred default ""
 *  @param[in] cstart Write color default moto::Color::NONE
 *  @param[in] cend End of range default moto::Color::NONE
 *  @param[in] tid Thread id default moto::intenal::DUMMY_TID ("---------------")
 */
inline auto write_clog(std::string_view msg, std::string now,
                       std::string_view tag = "MOTO_D",
                       std::string_view fname = "",
                       std::uint_least32_t line = 0,
                       const char* fun = "",
                       std::string_view cstart = Color::NONE,
                       std::string_view cend = Color::NONE,
                       std::string_view tid = DUMMY_TID
    ) noexcept -> void {
    std::osyncstream(std::clog) << std::format(
        "{}{} [{}] [{}]: {} {}:{}:{}{}\n", cstart, now, tid, tag,
        msg, fname, line, fun, cend);
}

inline auto log(std::string_view msg,
                src_info si = src_info::current()) {

    auto now = getCurrentClockCount();
    auto fname = removePath(si.file_name());
    write_clog(msg, now, "MOTO_D", fname, si.line(), "");
}

inline auto mark(src_info si = src_info::current()) -> void {
    auto now = getCurrentTime();
    auto fname = removePath(si.file_name());
    write_clog("----", now, "MARK ", fname, si.line(), si.function_name());
}

} //<-- namespace internal ends here.
} //<-- namespace ends here.

# define MARK() moto::internal::mark()

# if defined(MOTO_CORE_DEBUG)
#   define MOTO_LOG(msg) moto::internal::log(msg)
# else
#   define MOTO_LOG(msg)
# endif
///@}


#endif //<-- macro  MOTO_COMMON_HPP ends here.
