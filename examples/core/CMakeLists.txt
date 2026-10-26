#[[
 copyright © 2026 s3mat3
 This code is licensed under the MIT License, see the LICENSE file for details

 brief: test build for timer.hpp

 author s3mat3

]]
cmake_minimum_required (VERSION 3.24)
set(TARGET_BASE "moto_core")
project(
  example-${TARGET_BASE}
  VERSION ${MOTO_VERSION_STRING}
  LANGUAGES CXX
)

set(EXAMPLE_TARGET_SOURCES_BASE ${MOTO_EXAMPLES_BASE}/core)

set(EXECUTABLE_OUTPUT_PATH ${MOTO_EXAMPLES_OUT_DIR}/core)

# Enable threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# base includes
include_directories(
  ${MOTO_INCLUDE_BASE}
  ${MOTO_CORE}
)

# core exaples define
## debug
set(CORE_DEBUG_TARGET "debug_example")
## event_dispatcher
set(CORE_EVENT_DISPATCHER_TARGET "event_dispatcher_example")
## logger
set(CORE_LOGGER_TARGET "logger_example")
## result
set(CORE_RESULT_TARGET "result_example")
## timer
set(CORE_TIMER_TARGET "timer_example")


### final examples executable targets
####
#### debug
####
add_executable(
  ${CORE_DEBUG_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${CORE_DEBUG_TARGET}.cpp
)
###############################
# compile switch -std=c++23   #
###############################
# set_target_properties(
#   ${CORE_DEBUG_TARGET}
#   PROPERTIES
#   CXX_STANDARD 23
#   CXX_STANDARD_REQUIRED ON
#   # reject -std=g++23
#   CXX_EXTENSIONS OFF
# )
target_link_libraries(
  ${CORE_DEBUG_TARGET}
  # GCC
  PRIVATE stdc++
  # MSVC
  # PRIVATE std:c++latest
)
####
#### event_dispatcher
####
add_executable(
  ${CORE_EVENT_DISPATCHER_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${CORE_EVENT_DISPATCHER_TARGET}.cpp
)
####
#### logger
####
add_executable(
  ${CORE_LOGGER_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${CORE_LOGGER_TARGET}.cpp
  ${EXAMPLE_TARGET_SOURCES_BASE}/${CORE_LOGGER_TARGET}_other.cpp
)
####
#### result
####
add_executable(
  ${CORE_RESULT_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${CORE_RESULT_TARGET}.cpp
)
####
#### timer
####
add_executable(
  ${CORE_TIMER_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${CORE_TIMER_TARGET}.cpp
)
