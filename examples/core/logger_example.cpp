/**
 * @file example.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief
 *
 * @author s3mat3
 */
#define MOTO_USE_LOG 1
#include "logger.hpp"

void other_func();

int main() {
    std::cout << "set loglevel = D" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::D);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    std::cout << "set loglevel = I" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::I);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    std::cout << "set loglevel = N" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::N);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    std::cout << "set loglevel = W" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::W);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    std::cout << "set loglevel = E" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::E);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    std::cout << "set loglevel = F" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::F);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    std::cout << "set loglevel = V" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::V);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    std::cout << "set loglevel = D use_color = false" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::D);
    moto::Logger::instance().use_color(false);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");
    other_func();

    moto::internal::log("日本語もOK");
    MARK();
}

