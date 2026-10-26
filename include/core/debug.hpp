/**
 * @file debug.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief Debugging utilities for type demangling, variable dumping, and assertions.
 *
 * @author s3mat3
 */

#pragma once



#ifndef MOTO_DEBUG_HPP
# define  MOTO_DEBUG_HPP

# include <array>
# include <concepts>
# include <memory>
# include <sstream>
# include <string>
# include <typeinfo>
# include <type_traits>

# include "./internal/common.hpp"
# include "platform.hpp"
///@addtogroup core
///@{
// compile switch for get_backtrace
#if defined(__cpp_lib_stacktrace)  ///< Specifically indicates whether the
                                   // function is implemented
                                   // Even if the compiler is in C++23 mode,
                                   // if the library side is not implemented, safely fall back.
    #include <stacktrace>
#elif defined(MOTO_OS_POSIX)
    #include <execinfo.h>
#else // defined(MOTO_OS_WINDOWS)
    #include <windows.h>
    #include <dbghelp.h>
    #pragma comment(lib, "dbghelp.lib")
#endif

// compile switch for moto::debug::demangle
# if defined(__GNUC__) || defined(__clang__)
#  include <cxxabi.h>
# endif


namespace moto {
/// namespace debug
/// @example core/debug_example.cpp
namespace debug {

/** Demangles a mangled type name into a human-readable format.
 *
 * usage
 * @code
 * std::cout << "Type name is " << moto::debug::demangle(typeid(size_t).name())
 * << std::endl;
 * @endcode
 *
 * @param name The mangled type name provided by typeid().name().
 * @return A demangled string, or the original name if demangling fails or is
 * not supported.
*/
inline std::string demangle(const char* name) {
    if (! name) return "";
#if defined(__GNUC__) || defined(__clang__)
    // Itanium ABI (GCC, Clang)
    int status = 0;
    // By passing a custom deleter (free) to unique_ptr, it will be
    // automatically released when exiting the scope.
    std::unique_ptr<char, void(*)(void*)> res{
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : name;

#elif defined(_MSC_VER)
        // MSVC's typeid(T).name() is in human-readable format by default, so
        // it is returned as is.
        return name;
#else
        // Other environments
        return name;
#endif
}

/** Table of readable names for ASCII control characters (0x00 to 0x1F).
 */
static constexpr std::array<std::string_view, 32> ctrlNames = {
    "[NUL]", "[SOH]", "[STX]", "[ETX]", "[EOT]", "[ENQ]", "[ACK]", "[BEL]",
    "[BS ]",  "[HT ]",  "[LF ]",  "[VT ]",  "[FF ]",  "[CR ]",  "[SO ]",  "[SI ]",
    "[DC1]", "[DC2]", "[DC3]", "[DC4]", "[NAK]", "[SYN]", "[ETB]", "[CAN]",
    "[EM ]",  "[SUB]", "[ESC]", "[FS ]",  "[GS ]",  "[RS ]",  "[US ]", "[SPC]"
};


/** Converts a string containing non-printable characters into a readable
 * format.
 *
 * Control characters are replaced with markers like [NUL] or [SPC],
 * and high-bit characters are represented as hexadecimal.
 *
 * @param s The input string view to be processed.
 * @return A string where non-printable characters are replaced by
 * human-readable labels.
 */
static inline std::string toReadableCtrlCode(std::string_view s) {
    std::string result;
    /// Reserve to reduce re-allocation (leave some leeway as it will be
    /// longer than the original string)
    /// It has not been considered whether doubling is appropriate.
    result.reserve(s.size() * 2);

    for (const unsigned char c : s) {
        if (c < 0x20) {
            /// 0x00-0x1F: get from ctrlNames table
            result.append(ctrlNames[c]);
        } else if (c == 0x20) {
            /// space code
            result.append("[SPC]");
        } else if (c == 0x7f) {
            /// DEL
            result.append("[DEL]");
        } else if (c >= 0x80) {
            /// 0x80-0xFF: Visualization in hexadecimal notation
            if (c == 0xff) {
                result.append("[EOF]");
            } else {
                result.append(std::format("[{:02x}H]", c));
            }
        } else {
            /// normal displayable characters
            result.push_back(c);
        }
    }
    return result;
}


// --- Concepts ---

/** Concept for types that are convertible to string_view and are either
 * pointers or arrays (C-style strings).
 */
template <typename T>
concept CString = std::is_convertible_v<T, std::string_view> &&
                  (std::is_pointer_v<std::remove_cvref_t<T>> || std::is_array_v<std::remove_cvref_t<T>>);

/** Concept for types that implement a to_string() method returning a string.
 */
template <typename T>
concept HasToString = requires(T v) { { v.to_string() } -> std::convertible_to<std::string>; };

/** Concept for types that behave like maps (e.g., std::map,
 * std::unordered_map), containing key_type, mapped_type, and providing a
 * forward iterator.
 */
template <typename T>
concept IsMapLike = requires(T t) {
    typename T::mapped_type;
    typename T::key_type;
    { t.begin() } -> std::forward_iterator;
};

/** Concept for range-like types that are not maps, C-strings, or standard
 * strings.
 */
template <typename T>
concept IsRangeLike = std::ranges::range<T> &&
    !IsMapLike<T> &&
    !CString<T> &&
    !std::is_same_v<std::remove_cvref_t<T>, std::string>;

/** Converts a variable to a string representation similar to PHP's var_dump.
 *
 * Handles strings, C-strings, arithmetic types, map-like containers,
 * range-like containers, pointers, and classes with to_string() methods.
 *
 * @tparam T The type of the variable to dump.
 * @param v The variable to be converted.
 * @return A string representation of the variable's value and type.
 */
template <typename T>
inline auto dump_var_simple(const T& v) -> std::string {
    using DecayedT = std::remove_cvref_t<T>;

    // 1. std::string family
    if constexpr (std::is_same_v<DecayedT, std::string> || std::is_same_v<DecayedT, std::string_view>) {
        return std::string(v);
    }
    // 2. c style string
    else if constexpr (CString<DecayedT>) {
        return toReadableCtrlCode(v);
    }
    // 3. numeric
    else if constexpr (std::is_arithmetic_v<DecayedT>) {
        return std::to_string(v);
    }
    // 4. Map (unordered_map)
    else if constexpr (IsMapLike<DecayedT>) {
        std::stringstream ss;
        ss << "map(";
        bool first = true;
        for (const auto& [key, value] : v) {
            if (!first) ss << ", ";
            ss << "[" << dump_var_simple(key) << " => " << dump_var_simple(value) << "]";
            first = false;
        }
        ss << ")";
        return ss.str();
    }
    // 5. Range (vector, list)
    else if constexpr (IsRangeLike<DecayedT>) {
        std::stringstream ss;
        ss << "seq(";
        bool first = true;
        for (const auto& item : v) {
            if (!first) ss << ", ";
            ss << dump_var_simple(item);
            first = false;
        }
        ss << ")";
        return ss.str();
    }
    // 6. pointer
    else if constexpr (std::is_pointer_v<DecayedT>) {
        std::stringstream ss;
        ss << v;
        if (v == nullptr) return "nullptr @0x0";
        if constexpr (std::is_same_v<DecayedT, void*>) {
            return "void* @" + ss.str();
        } else {
            return dump_var_simple(*v) + " @" + ss.str();
        }
    }
    // 7. class
    else if constexpr (std::is_class_v<DecayedT>) {
        if constexpr (HasToString<DecayedT>) return v.to_string();
        else return "class: " + demangle(typeid(DecayedT).name());
    }
    else {
        return "unknown";
    }
}

/** Formats a variable name and its value as a string.
 *
 * @tparam T The type of the variable.
 * @param vn The name of the variable.
 * @param v The value of the variable.
 * @return A string in the format "name => value".
 */
template <typename T>
inline auto to_string_var(const char* vn, const T& v) -> std::string {
    return std::format("{} => {}", vn, moto::debug::dump_var_simple(v));
}

/** Prints a variable's name and value to clog with terminal color support.
 *
 * @tparam T The type of the variable.
 * @param vn The name of the variable.
 * @param v The value of the variable.
 */
template <typename T>
inline auto print_var_simple(const char* vn, const T& v) -> void {
    auto cc = ColorConfig{};
    auto cstart = cc.auto_detect_terminal() ? moto::Color::CYAN : moto::Color::NONE;
    auto cend = (cstart != moto::Color::NONE) ? moto::Color::RESET : moto::Color::NONE;
    std::osyncstream(std::clog) << std::format("{}{}{}\n", cstart, to_string_var(vn, v), cend);
}

/** Prints a formatted assertion failure message to clog.
 *
 * @param cond The string representation of the condition that failed.
 * @param msg An optional custom error message.
 * @param loc The source location where the assertion occurred.
 */
inline auto print_assertion(
    const char* cond, std::string_view msg = "ASSERT",
    std::source_location loc = std::source_location::current()) -> void {
    auto cc = ColorConfig{};
    auto fname = moto::removePath(loc.file_name());
    auto cstart = cc.auto_detect_terminal() ? moto::Color::RED : moto::Color::NONE;
    auto cend = (cstart != moto::Color::NONE) ? moto::Color::RESET : moto::Color::NONE;
    std::osyncstream(std::clog)
        << std::format("{}=== Assertion occurred ===\n\t {} => {} in {}:{}:{}{}\n",
                       cstart, cond, msg, loc.function_name(), fname, loc.line(),cend);
}

/** Evaluates a condition and prints an assertion failure if it is false.
 *
 * @tparam T The type of the condition.
 * @param cond The condition to check.
 * @param cond_str The string representation of the condition.
 * @param msg The error message to display upon failure.
 * @param loc The source location of the call.
 * @return true if the condition is true, false otherwise.
 */
template <typename T>
constexpr bool check_assertion(T cond, const char* cond_str, std::string_view msg,
                               std::source_location loc = std::source_location::current()) {
    if (!cond) {
        print_assertion(cond_str, msg, loc);
        return false;
    }
    return true;
}
/** Captures the current call stack (backtrace) and returns it as a string.
 *
 * This function selects the implementation based on the environment in the
 * following priority:
 * 1. C++23 std::stacktrace (Standard Library)
 * 2. Windows API (CaptureStackBackTrace / DbgHelp)
 * 3. POSIX glibc/macOS (backtrace / backtrace_symbols)
 *
 * @note **C++23 Implementation Notes:**
 * When using `std::stacktrace`, additional library linking may be required
 * depending on the compiler:
 * - GCC: Link with `-lstdc++_libbacktrace`.
 * - MSVC: Use the `/std:c++latest` option.
 *
 * @note **Symbol Resolution:**
 * - On Windows, function names may be replaced by memory addresses if the
 * corresponding `.pdb` (debug information) file is not found in the execution
 * directory.
 * - On POSIX environments, detailed symbol resolution may be limited if the
 * binary was not compiled with debug information (e.g., missing `-g` flag).
 *
 * @note Time complexity is proportional to the stack depth (up to 64
 * frames). Memory allocation depends on std::string dynamic growth and,
 * in POSIX environments, the internal malloc used by backtrace_symbols.
 *
 * @return A std::string containing the captured backtrace.
 *  Returns an error message (e.g., "backtrace failed") if the capture fails.
 */
inline auto get_backtrace() -> std::string {
    std::string ret = "--- TRACE ---\n";

    // 1. When C++23 std::stacktrace can be used
#  if defined(__cpp_lib_stacktrace)
    auto trace = std::stacktrace::current();
    if (trace.empty()) return "stacktrace empty";
    for (const auto& entry : trace) {
        ret += std::format("\t==> {}\n", std::to_string(entry));
    }

    // 2. When C++23 cannot be used -> Go to OS specific implementation
#  elif defined(MOTO_OS_POSIX)
    void* frames[64];
    const int frame_size = backtrace(frames, 64);
    if (frame_size <= 0) return "backtrace failed: no trace info";

    using symbols_p = std::unique_ptr<char*[], void(*)(void*)>;
    symbols_p symbols(backtrace_symbols(frames, frame_size), std::free);
    if (!symbols) return "backtrace_symbols failed: no symbols";

    for (int i = 0; i < frame_size; ++i) {
        ret += std::format("\t==> {}\n", symbols[i]);
    }
#  elif defined(MOTO_OS_WINDOWS)
    void* frames[64];
    unsigned short frame_count = CaptureStackBackTrace(0, 64, frames, nullptr);
    if (frame_count == 0) return "backtrace failed";

    SymInitialize(GetCurrentProcess(), nullptr, TRUE);
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    for (unsigned int i = 0; i < frame_count; ++i) {
        DWORD64 displacement = 0;
        if (SymFromAddr(GetCurrentProcess(), (DWORD64)frames[i], &displacement, symbol)) {
            ret += std::format("\t==> {}\n", symbol->Name);
        } else {
            ret += std::format("\t==> 0x{:X}\n", (uintptr_t)frames[i]);
        }
    }
#endif

    return ret;
} //<-- function get_backtrace ends here.

} //<-- namespace debug ends here.
} //<-- namespace moto ends here.

/// debug macros
# if defined(MOTO_USE_LOG)
#  define MOTO_VAR_DUMP(v) moto::debug::print_var_simple((#v), v)
#  define MOTO_VAR_STRING(v) moto::debug::to_string_var((#v), v)
# else
#  define MOTO_VAR_DUMP(v)
#  define MOTO_VAR_STRING(v) ""
# endif

# if defined(MOTO_USE_ASSERT)
#  define MOTO_ASSERT(cond, msg) \
        moto::debug::check_assertion((cond), #cond, (msg), std::source_location::current())
# else
#  define MOTO_ASSERT(cond, msg) true
# endif

# if defined(MOTO_USE_TRACE)
#  define MOTO_TRACE() moto::debug::get_backtrace()
# else
#  define MOTO_TRACE() ""
# endif
///@}
#endif //<-- macro  MOTOy_DEBUG_HPP ends here.
