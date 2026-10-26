#[[
 copyright © 2026 s3mat3
 This code is licensed under the MIT License, see the LICENSE file for details

 brief: test build for core unit tests.

 author s3mat3

]]
cmake_minimum_required (VERSION 3.24)
set(MODULE_BASE "core")
set(TARGET_BASE "moto_${MODULE_BASE}")
project(
  unit-test-${TARGET_BASE}
  VERSION ${MOTO_VERSION_STRING}
  LANGUAGES CXX
)

set(TEST_TARGET_SOURCES_BASE ${MOTO_TESTS_BASE}/${MODULE_BASE})

set(EXECUTABLE_OUTPUT_PATH ${MOTO_TESTS_OUT_DIR}/${TARGET_BASE})

# Enable threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# base includes
include_directories(
  ${MOTO_INCLUDE_BASE}
  ${MOTO_CORE}
  ${MOTO_TESTER_BASE}
)
# core tests define
## debug
set(CORE_DEBUG_TARGET "debug_test")
## event_dispatcher
set(CORE_EVENT_DISPATCHER_TARGET "event_dispatcher_test")
## logger
set(CORE_LOGGER_TARGET "logger_test")
## result
set(CORE_RESULT_TARGET "result_test")
## timer
set(CORE_TIMER_TARGET "timer_test")


### final executable targets
####
#### debug
####
add_executable(
  ${CORE_DEBUG_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${CORE_DEBUG_TARGET}.cpp
)
add_test(
  NAME ${CORE_DEBUG_TARGET}
  COMMAND ${CORE_DEBUG_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
####
#### event_dispatcher
####
add_executable(
  ${CORE_EVENT_DISPATCHER_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${CORE_EVENT_DISPATCHER_TARGET}.cpp
)
add_test(
  NAME ${CORE_EVENT_DISPATCHER_TARGET}
  COMMAND ${CORE_EVENT_DISPATCHER_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
####
#### logger
####
add_executable(
  ${CORE_LOGGER_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${CORE_LOGGER_TARGET}.cpp
)
add_test(
  NAME ${CORE_LOGGER_TARGET}
  COMMAND ${CORE_LOGGER_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
####
#### result
####
add_executable(
  ${CORE_RESULT_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${CORE_RESULT_TARGET}.cpp
)
####################################################
# if use std::expected compile switch -std=c++23   #
####################################################
set_target_properties(
  ${CORE_RESULT_TARGET}
  PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  # reject -std=g++23
  CXX_EXTENSIONS OFF
)

add_test(
  NAME ${CORE_RESULT_TARGET}
  COMMAND ${CORE_RESULT_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
####
#### timer
####
add_executable(
  ${CORE_TIMER_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${CORE_TIMER_TARGET}.cpp
)
target_link_libraries(
  ${CORE_TIMER_TARGET}
  PRIVATE Threads::Threads
)
add_test(
  NAME ${CORE_TIMER_TARGET}
  COMMAND ${CORE_TIMER_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)

