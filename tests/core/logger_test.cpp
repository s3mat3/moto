/**
 * @file logger_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for logger.hpp
 *
 * @author s3mat3
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "logger.hpp"
#include <sstream>
#include <iostream>

TEST_CASE("Testing moto::index2LogLevel") {
    CHECK(moto::index2LogLevel(0) == moto::LOG_LEVEL::V);
    CHECK(moto::index2LogLevel(1) == moto::LOG_LEVEL::F);
    CHECK(moto::index2LogLevel(2) == moto::LOG_LEVEL::E);
    CHECK(moto::index2LogLevel(3) == moto::LOG_LEVEL::W);
    CHECK(moto::index2LogLevel(4) == moto::LOG_LEVEL::N);
    CHECK(moto::index2LogLevel(5) == moto::LOG_LEVEL::I);
    CHECK(moto::index2LogLevel(6) == moto::LOG_LEVEL::D);
    CHECK(moto::index2LogLevel(99) == moto::LOG_LEVEL::D); // default case
    CHECK(moto::index2LogLevel(-1) == moto::LOG_LEVEL::D); // default case
}

TEST_CASE("Testing moto::removePath") {
    SUBCASE("POSIX path") {
        CHECK(moto::removePath("/home/user/project/main.cpp") == "main.cpp");
    }
    SUBCASE("Windows path") {
        CHECK(moto::removePath("C:\\Users\\Admin\\test.hpp") == "test.hpp");
    }
    SUBCASE("No path") {
        CHECK(moto::removePath("logger.hpp") == "logger.hpp");
    }
    SUBCASE("Empty string") {
        CHECK(moto::removePath("") == "");
    }
}

TEST_CASE("Testing moto::get_color_for_level") {
    CHECK(moto::get_color_for_level(moto::LOG_LEVEL::F) == moto::Color::RED);
    CHECK(moto::get_color_for_level(moto::LOG_LEVEL::E) == moto::Color::RED);
    CHECK(moto::get_color_for_level(moto::LOG_LEVEL::W) == moto::Color::YELLOW);
    CHECK(moto::get_color_for_level(moto::LOG_LEVEL::N) == moto::Color::GREEN);
    CHECK(moto::get_color_for_level(moto::LOG_LEVEL::I) == moto::Color::BLUE);
    CHECK(moto::get_color_for_level(moto::LOG_LEVEL::D) == moto::Color::NONE);
}

TEST_CASE("Testing moto::extract_func_name_neadle") {
    auto loc = std::source_location::current();

    SUBCASE("Debug level should return function name") {
        std::string  name = moto::extract_func_name_neadle(moto::LOG_LEVEL::D, loc);
        CHECK_FALSE(name.empty());
    }
    SUBCASE("Non-Debug level should return empty string") {
        CHECK(moto::extract_func_name_neadle(moto::LOG_LEVEL::I, loc) == "");
        CHECK(moto::extract_func_name_neadle(moto::LOG_LEVEL::F, loc) == "");
    }
}

TEST_CASE("Testing Levl. DEBUG") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::D);

    SUBCASE("Log message contains the provided text") {
        std::string_view msg = "Fatal log message";
        Logger::instance().log(LOG_LEVEL::F, msg);

        std::string output = buffer.str();
        CHECK(output.find(std::string(msg)) != std::string::npos);
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::F)]) != std::string::npos);
    }
    SUBCASE("Debug log contains function name") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::D, "Debug trace");

        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::D)]) != std::string::npos);
        CHECK_FALSE(output.empty());
    }

    std::clog.rdbuf(old_clog);
}


TEST_CASE("Testing Levl. DEBUG") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::D);
    // Fatal
    SUBCASE("Log message contains the provided Fatal") {
        buffer.str(""); // clear buffer
        std::string_view msg = "Test log message";
        Logger::instance().log(LOG_LEVEL::F, msg);

        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::F)]) != std::string::npos);
    }
    // Error
    SUBCASE("Log message contains the provided Error") {
        buffer.str(""); // clear buffer
        std::string_view msg = "Test log message";
        Logger::instance().log(LOG_LEVEL::E, msg);

        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::E)]) != std::string::npos);
    }
    // Warn
    SUBCASE("Log message contains the provided Warn") {
        buffer.str(""); // clear buffer
        std::string_view msg = "Test log message";
        Logger::instance().log(LOG_LEVEL::W, msg);

        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::W)]) != std::string::npos);
    }
    // Notice
    SUBCASE("Log message contains the provided Notice") {
        buffer.str(""); // clear buffer
        std::string_view msg = "Test log message";
        Logger::instance().log(LOG_LEVEL::N, msg);

        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::N)]) != std::string::npos);
    }
    // Info
    SUBCASE("Log message contains the provided Info") {
        buffer.str(""); // clear buffer
        std::string_view msg = "Test log message";
        Logger::instance().log(LOG_LEVEL::I, msg);

        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::I)]) != std::string::npos);
    }
    // Debug
    SUBCASE("Log message contains the provided Debug") {
        buffer.str(""); // clear buffer
        std::string_view msg = "Test log message";
        Logger::instance().log(LOG_LEVEL::D, msg);

        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::D)]) != std::string::npos);
    }
    std::clog.rdbuf(old_clog);
}

TEST_CASE("Testing Levl. INFO") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::I);
    // Fatal
    SUBCASE("Log message contains the provided Fatal") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::F, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::F)]) != std::string::npos);
    }
    // Error
    SUBCASE("Log message contains the provided Error") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::E, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::E)]) != std::string::npos);
    }
    // Warn
    SUBCASE("Log message contains the provided Warn") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::W, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::W)]) != std::string::npos);
    }
    // Notice
    SUBCASE("Log message contains the provided Notice") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::N, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::N)]) != std::string::npos);
    }
    // Info
    SUBCASE("Log message contains the provided Info") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::I, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::I)]) != std::string::npos);
    }
    // Debug
    SUBCASE("Log message is empty") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::D, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    std::clog.rdbuf(old_clog);
}

TEST_CASE("Testing Levl. NOTICE") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::N);
    // Fatal
    SUBCASE("Log message contains the provided Fatal") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::F, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::F)]) != std::string::npos);
    }
    // Error
    SUBCASE("Log message contains the provided Error") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::E, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::E)]) != std::string::npos);
    }
    // Warn
    SUBCASE("Log message contains the provided Warn") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::W, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::W)]) != std::string::npos);
    }
    // Notice
    SUBCASE("Log message contains the provided Notice") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::N, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::N)]) != std::string::npos);
    }
    // Info
    SUBCASE("Log message contains the provided Info") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::I, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Debug
    SUBCASE("Log message is empty") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::D, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    std::clog.rdbuf(old_clog);
}

TEST_CASE("Testing Levl. WARN") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::W);
    // Fatal
    SUBCASE("Log message contains the provided Fatal") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::F, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::F)]) != std::string::npos);
    }
    // Error
    SUBCASE("Log message contains the provided Error") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::E, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::E)]) != std::string::npos);
    }
    // Warn
    SUBCASE("Log message contains the provided Warn") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::W, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::W)]) != std::string::npos);
    }
    // Notice
    SUBCASE("Log message contains the provided Notice") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::N, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Info
    SUBCASE("Log message contains the provided Info") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::I, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Debug
    SUBCASE("Log message is empty") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::D, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    std::clog.rdbuf(old_clog);
}

TEST_CASE("Testing Levl. ERROR") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::E);
    // Fatal
    SUBCASE("Log message contains the provided Fatal") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::F, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::F)]) != std::string::npos);
    }
    // Error
    SUBCASE("Log message contains the provided Error") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::E, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::E)]) != std::string::npos);
    }
    // Warn
    SUBCASE("Log message contains the provided Warn") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::W, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Notice
    SUBCASE("Log message contains the provided Notice") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::N, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Info
    SUBCASE("Log message contains the provided Info") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::I, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Debug
    SUBCASE("Log message is empty") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::D, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    std::clog.rdbuf(old_clog);
}

TEST_CASE("Testing Levl. FATAL") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::F);
    // Fatal
    SUBCASE("Log message contains the provided Fatal") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::F, "Test log message");
        std::string output = buffer.str();
        CHECK(output.find(LogLevelNameList[LogLevel2index(LOG_LEVEL::F)]) != std::string::npos);
    }
    // Error
    SUBCASE("Log message contains the provided Error") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::E, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Warn
    SUBCASE("Log message contains the provided Warn") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::W, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Notice
    SUBCASE("Log message contains the provided Notice") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::N, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Info
    SUBCASE("Log message contains the provided Info") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::I, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Debug
    SUBCASE("Log message is empty") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::D, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    std::clog.rdbuf(old_clog);
}

TEST_CASE("Testing Levl. VOID") {
    using namespace moto;
    // Preparing to capture std::clog output
    std::stringstream buffer;
    std::streambuf* old_clog = std::clog.rdbuf(buffer.rdbuf());
    // Setup logger
    Logger::instance().level(LOG_LEVEL::V);
    // Fatal
    SUBCASE("Log message contains the provided Fatal") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::F, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Error
    SUBCASE("Log message contains the provided Error") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::E, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Warn
    SUBCASE("Log message contains the provided Warn") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::W, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Notice
    SUBCASE("Log message contains the provided Notice") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::N, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Info
    SUBCASE("Log message contains the provided Info") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::I, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    // Debug
    SUBCASE("Log message is empty") {
        buffer.str(""); // clear buffer
        Logger::instance().log(LOG_LEVEL::D, "Test log message");
        std::string output = buffer.str();
        CHECK(output.empty());
    }
    std::clog.rdbuf(old_clog);
}
