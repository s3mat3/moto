#[[
 copyright © 2026 s3mat3
 This code is licensed under the MIT License, see the LICENSE file for details

 brief: test build for timer.hpp

 author s3mat3

]]
cmake_minimum_required (VERSION 3.24)
set(TARGET_BASE "moto_ext")
project(
  example-${TARGET_BASE}
  VERSION ${MOTO_VERSION_STRING}
  LANGUAGES CXX
)

set(EXAMPLE_TARGET_SOURCES_BASE ${MOTO_EXAMPLES_BASE}/ext)

set(EXECUTABLE_OUTPUT_PATH ${MOTO_EXAMPLES_OUT_DIR}/ext)

# Enable threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# base includes
include_directories(
  ${MOTO_INCLUDE_BASE}
  ${MOTO_CORE}
  ${MOTO_EXT}
)

# ext examples
## event_bus
set(EXT_EVENT_BUS_TARGET "event_bus_example")
## fsm
set(EXT_FSM_TARGET "fsm_example")
## thread_pool
set(EXT_THREAD_POOL_TARGET "thread_pool_example")

### final executable target
####
#### event_bus
####
add_executable(
  ${EXT_EVENT_BUS_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${EXT_EVENT_BUS_TARGET}.cpp
)
####
#### fsm
####
add_executable(
  ${EXT_FSM_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${EXT_FSM_TARGET}.cpp
)
####
#### thread_pool
####
add_executable(
  ${EXT_THREAD_POOL_TARGET}
  ${EXAMPLE_TARGET_SOURCES_BASE}/${EXT_THREAD_POOL_TARGET}.cpp
)

target_link_libraries(
  ${EXT_THREAD_POOL_TARGET}
  PRIVATE Threads::Threads
  PRIVATE dl
)
