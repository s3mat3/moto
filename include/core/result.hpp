/**
 * @file result.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief A return value type class aimed at something similar to the return value of the Go language.
 *
 * @author s3mat3
 */

#pragma once

#ifndef MOTO_RESULT_HPP
# define  MOTO_RESULT_HPP

//# include "./internal/common.hpp"
# include <format>
# include <stdexcept>
# include <string>
# include <variant>

///@addtogroup core
///@{
// C++23 std::expected check
/** MOTO_HAS_EXPECTED
 * Feature flag to determine if std::expected (C++23) is available.
 */
#if __has_include(<expected>) && (__cplusplus >= 202302L)
# include <expected>
# define MOTO_HAS_EXPECTED 1
#else
# define MOTO_HAS_EXPECTED 0
#endif

namespace moto {

/** Error
 *
 * Represents an error state containing a numeric code and a descriptive cause.
 */
struct Error {
    int code;
    std::string cause;

    Error(int code, const std::string& c, std::string_view chain = "")
        : code(code), cause(c) {
        if (!chain.empty()) {
            cause.append(std::format(", from: {}", chain));
        }
    }

    Error(const std::error_code& ec, const std::string& context, std::string_view chain = "")
        : code(ec.value()), 
          cause(std::format("{}: {}", context, ec.message())) {
        if (!chain.empty()) {
            cause.append(std::format(", from: {}", chain));
        }
    }

    /**
     * Constructs an Error object from the current global errno.
     * @param context Additional context information to describe where the error occurred.
     * @param chain An optional error chain for tracing.
     * @return An Error object wrapping the current errno.
     */
    static Error from_errno(const std::string& context, std::string_view chain = "") {
        return Error(std::error_code(errno, std::generic_category()), context, chain);
    }
}; //<-- struct Error ends here.

/** Result
 * A wrapper class that holds either a successful value of type T or an error
 * of type E.
 * This provides a Go-like error handling mechanism for C++20 and C++23.
 *
 * @tparam T The type of the value returned on success.
 * @tparam E The type of the error returned on failure. Defaults to moto::Error.
 */
template <typename T, typename E = Error>
class Result {
private:
#if MOTO_HAS_EXPECTED
    std::expected<T, E> m_storage;
#else
    std::variant<T, E> m_storage;
#endif

public:
    using value_type = T;

    // --- Success constructors ---

    constexpr Result(T&& value)
        : m_storage(std::move(value)) {}

    constexpr Result(const T& value)
        : m_storage(value) {}

    // --- Error constructors ---

    /** Constructs a Result representing failure using an error object. */
    constexpr Result(E&& error)
#if MOTO_HAS_EXPECTED
        : m_storage(std::unexpected(std::move(error))) {}
#else
        : m_storage(std::move(error)) {}
#endif

    constexpr Result(const E& error) 
#if MOTO_HAS_EXPECTED
        : m_storage(std::unexpected(error)) {}
#else
        : m_storage(error) {}
#endif

    /** 
     * Factory method to create a failure Result from errno.
     * Only available if E is moto::Error.
     */
    static constexpr Result fail_errno(const std::string& context, std::string_view chain = "") {
        if constexpr (std::is_same_v<E, Error>) {
            return Result(Error::from_errno(context, chain));
        } else {
            static_assert(std::is_same_v<E, Error>, "fail_errno requires E to be moto::Error");
        }
    }

    /** 
     * Factory method to create a failure Result from a custom code and message.
     * Only available if E is moto::Error.
     */
    static constexpr Result fail(int code, const std::string& cause, std::string_view chain = "") {
        if constexpr (std::is_same_v<E, Error>) {
            return Result(Error(code, cause, chain));
        } else {
            static_assert(std::is_same_v<E, Error>, "fail requires E to be moto::Error");
        }
    }

    // --- Accessors ---

    constexpr bool has_value() const noexcept {
#if MOTO_HAS_EXPECTED
        return m_storage.has_value();
#else
        return std::holds_alternative<T>(m_storage);
#endif
    }

    constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    constexpr T& value() {
        if (!has_value()) throw std::runtime_error("Result: access value while holding error");
#if MOTO_HAS_EXPECTED
        return m_storage.value();
#else
        return std::get<T>(m_storage);
#endif
    }

    constexpr const T& value() const {
        if (!has_value()) throw std::runtime_error("Result: access value while holding error");
#if MOTO_HAS_EXPECTED
        return m_storage.value();
#else
        return std::get<T>(m_storage);
#endif
    }

    constexpr E& error() {
        if (has_value()) throw std::runtime_error("Result: access error while holding value");
#if MOTO_HAS_EXPECTED
        return m_storage.error();
#else
        return std::get<E>(m_storage);
#endif
    }

    constexpr const E& error() const {
        if (has_value()) throw std::runtime_error("Result: access error while holding value");
#if MOTO_HAS_EXPECTED
        return m_storage.error();
#else
        return std::get<E>(m_storage);
#endif
    }
};

/** Result<void, E>
 * Specialization of Result for cases where no success value is needed.
 */
template <typename E>
class Result<void, E> {
private:
#if MOTO_HAS_EXPECTED
    std::expected<void, E> m_storage;
#else
    std::variant<std::monostate, E> m_storage;
#endif

public:
    using value_type = void;

    constexpr Result()
#if MOTO_HAS_EXPECTED
        : m_storage() {}
#else
        : m_storage(std::monostate{}) {}
#endif

    constexpr Result(E&& error)
#if MOTO_HAS_EXPECTED
        : m_storage(std::unexpected(std::move(error))) {}
#else
        : m_storage(std::move(error)) {}
#endif

    constexpr Result(const E& error)
#if MOTO_HAS_EXPECTED
        : m_storage(std::unexpected(error)) {}
#else
        : m_storage(error) {}
#endif

    /** Factory method to create a failure Result from errno. */
    static constexpr Result fail_errno(const std::string& context, std::string_view chain = "") {
        if constexpr (std::is_same_v<E, Error>) {
            return Result(Error::from_errno(context, chain));
        } else {
            static_assert(std::is_same_v<E, Error>, "fail_errno requires E to be moto::Error");
        }
    }

    /** Factory method to create a failure Result from a custom code and message. */
    static constexpr Result fail(int code, const std::string& cause, std::string_view chain = "") {
        if constexpr (std::is_same_v<E, Error>) {
            return Result(Error(code, cause, chain));
        } else {
            static_assert(std::is_same_v<E, Error>, "fail requires E to be moto::Error");
        }
    }

    constexpr bool has_value() const noexcept {
#if MOTO_HAS_EXPECTED
        return m_storage.has_value();
#else
        return std::holds_alternative<std::monostate>(m_storage);
#endif
    }

    constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    constexpr E& error() {
        if (has_value()) throw std::runtime_error("Result: access error while holding value");
#if MOTO_HAS_EXPECTED
        return m_storage.error();
#else
        return std::get<E>(m_storage);
#endif
    }

    constexpr const E& error() const {
        if (has_value()) throw std::runtime_error("Result: access error while holding value");
#if MOTO_HAS_EXPECTED
        return m_storage.error();
#else
        return std::get<E>(m_storage);
#endif
    }
}; //<-- class Result ends here.

template <typename T>
struct is_result : std::false_type {};

template <typename T, typename E>
struct is_result<Result<T, E>> : std::true_type {};

template <typename E>
struct is_result<Result<void, E>> : std::true_type {};

template <typename T>
inline constexpr bool is_result_v = is_result<T>::value;


} //<-- namespace moto ends here.

///@}
#endif // MOTO_RESULT_HPP
