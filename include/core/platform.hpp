/**
 * @file platform.hpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief Define platform selector
 *
 * @author s3mat3
 */

#pragma once

#ifndef MOTO_PLATFORM_HPP
# define  MOTO_PLATFORM_HPP

///@addtogroup core
///@{
#if defined(_WIN32) || defined(_WIN64)
    #define MOTO_OS_WINDOWS
#else
    #define MOTO_OS_POSIX
#endif
///@}


#endif //<-- macro  MOTO_PLATFORM_HPP ends here.
