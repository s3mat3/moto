/**
 * @file other.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief
 *
 * @author s3mat3
 */

#include "logger.hpp"

void other_func() {
    std::cout << "other.cpp set loglevel = D" << std::endl;
    moto::Logger::instance().level(moto::LOG_LEVEL::D);
    moto::Logger::instance().use_color(true);
    LOG_F("Fatal");
    LOG_E("Error");
    LOG_W("Warn");
    LOG_N("Notice");
    LOG_I("Info");
    LOG_D("Debug");

}

