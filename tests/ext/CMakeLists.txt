#[[
 copyright © 2026 s3mat3
 This code is licensed under the MIT License, see the LICENSE file for details

 brief: tests build for ext

 author s3mat3

]]
cmake_minimum_required (VERSION 3.24)
set(MODULE_BASE "ext")
set(TARGET_BASE "moto_${MODULE_BASE}")
project(
  unit-test-${TARGET_BASE}
  VERSION ${MOTO_VERSION_STRING}
  LANGUAGES CXX
)

set(TEST_TARGET_SOURCES_BASE ${MOTO_TESTS_BASE}/${MODULE_BASE})

set(EXECUTABLE_OUTPUT_PATH ${MOTO_TESTS_OUT_DIR}/${MODULE_BASE})

# Enable threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# base includes
include_directories(
  ${MOTO_INCLUDE_BASE}
  ${MOTO_CORE}
  # ${MOTO_EXT}
  ${MOTO_TESTER_BASE}
)

# test define
## event_bus
set(EXT_EVENT_BUS_TARGET "event_bus_test")
## fsm
set(EXT_FSM_TARGET "fsm_test")
## thread_pool
set(EXT_THREAD_POOL_TARGET "thread_pool_test")

### final executable target
####
#### event_bus
####
add_executable(
  ${EXT_EVENT_BUS_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${EXT_EVENT_BUS_TARGET}.cpp
)
target_link_libraries(
  ${EXT_EVENT_BUS_TARGET}
  PRIVATE Threads::Threads
)
add_test(
  NAME ${EXT_EVENT_BUS_TARGET}
  COMMAND ${EXT_EVENT_BUS_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
####
#### fsm
####
add_executable(
  ${EXT_FSM_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${EXT_FSM_TARGET}.cpp
)
add_test(
  NAME ${EXT_FSM_TARGET}
  COMMAND ${EXT_FSM_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
####
#### thread_pool
####
add_executable(
  ${EXT_THREAD_POOL_TARGET}
  ${TEST_TARGET_SOURCES_BASE}/${EXT_THREAD_POOL_TARGET}.cpp
)

target_link_libraries(
  ${EXT_THREAD_POOL_TARGET}
  PRIVATE Threads::Threads
  PRIVATE dl
)

add_test(
  NAME ${EXT_THREAD_POOL_TARGET}
  COMMAND ${EXT_THREAD_POOL_TARGET}
  # CONFIGURATIONS Release
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)

