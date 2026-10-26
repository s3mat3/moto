/**
 * @file result_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for result.hpp
 *
 * @author s3mat3
 */


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "result.hpp"
#include <fstream>

TEST_CASE("moto::Result basic functionality") {
    using moto::Result;
    using moto::Error;

    SUBCASE("Success case") {
        // Initialize with success value
        Result<int> res = 100;

        CHECK(res.has_value() == true);
        CHECK(static_cast<bool>(res) == true);
        CHECK(res.value() == 100);

        // Confirm value rewriting
        res.value() = 200;
        CHECK(res.value() == 200);
    }

    SUBCASE("Error case") {
        // Initialize with error value
        Error err{500, "Internal Server Error"};
        Result<int> res = err;

        CHECK(res.has_value() == false);
        CHECK(static_cast<bool>(res) == false);
        CHECK(res.error().code == 500);
        CHECK(res.error().cause == "Internal Server Error");
    }

    SUBCASE("Exception handling") {
        // Confirm that an exception occurs when calling error() when there is a value
        Result<int> success_res = 10;
        CHECK_THROWS_AS(success_res.error(), std::runtime_error);

        // Confirm that an exception occurs when calling value() when there is an error
        Result<int> error_res = Error{404, "Not Found"};
        CHECK_THROWS_AS(error_res.value(), std::runtime_error);
    }
}

TEST_CASE("moto::Result extended factory methods") {
    using moto::Result;
    using moto::Error;

    SUBCASE("Using Result::fail") {
        // Test creating failure via static fail method
        auto res = Result<int>::fail(403, "Forbidden Access");

        CHECK_FALSE(res.has_value());
        CHECK(res.error().code == 403);
        CHECK(res.error().cause == "Forbidden Access");
    }

    SUBCASE("Using Result::fail_errno") {
        // Simulate an errno event by attempting to open a non-existent file
        {
            std::ifstream ifs("/non_existent_file_moto_test.txt");
        }
        // The above operation sets errno

        auto res = Result<int>::fail_errno("File open failure");

        CHECK_FALSE(res.has_value());
        // Check if the error code matches the current errno
        CHECK(res.error().code == errno);
        // Ensure the context string is present in the cause
        CHECK(res.error().cause.find("File open failure") != std::string::npos);
    }
}

TEST_CASE("moto::Result void specialization") {
    using moto::Result;
    using moto::Error;

    SUBCASE("Success void") {
        Result<void> res; // Default constructor is success
        CHECK(res.has_value() == true);
        CHECK(static_cast<bool>(res) == true);
        CHECK_THROWS_AS(res.error(), std::runtime_error);
    }

    SUBCASE("Error void") {
        auto res = Result<void>::fail(500, "Void Error");
        CHECK(res.has_value() == false);
        CHECK(res.error().code == 500);
        CHECK(res.error().cause == "Void Error");
    }
}

TEST_CASE("moto::Result with different types") {
    using moto::Result;
    using moto::Error;

    SUBCASE("String value and custom error") {
        Result<std::string> res = std::string("Hello World");
        CHECK(res.value() == "Hello World");

        res = Error{1, "Fail"};
        CHECK(res.error().cause == "Fail");
    }

    SUBCASE("Custom Error type") {
        // Change the error type to int and use
        Result<double, int> res = 404; 
        CHECK_FALSE(res.has_value());
        CHECK(res.error() == 404);
    }
}

TEST_CASE("moto::Result move semantics") {
    using moto::Result;
    using moto::Error;

    SUBCASE("Move constructor") {
        Result<std::string> res1 = std::string("Heavy String Data");
        Result<std::string> res2 = std::move(res1);

        CHECK(res2.value() == "Heavy String Data");
    }
}
