#[[
 copyright © 2026 s3mat3
 This code is licensed under the MIT License, see the LICENSE file for details

  -DCMAKE_BUILD_TYPE=(Debug | Release | Relwithdebinfo | Memcheck)
  -DCMAKE_EXPORT_COMPILE_COMMANDS=on
  -DCMAKE_VERBOSE_MAKEFILE:BOOL=TRUE
  -DMOTO_BUILD_TEST=ON (default ON) we will compile code
  -DMOTO_BUILD_EXAMPLES=ON (default ON) we will compile example code
  -DMOTO_BUILD_DOC=ON (default ON) we will generate source document

 brief: Top build script for moto

 author s3mat3

]]
cmake_minimum_required (VERSION 3.24)

file(READ VERSION MOTO_VERSION_STRING)
string(STRIP ${MOTO_VERSION_STRING} MOTO_VERSION_STRING)
set(PROJECT_NAME "moto")
project(
  ${PROJECT_NAME}
  VERSION ${MOTO_VERSION_STRING}
  LANGUAGES CXX
  )

option(MOTO_BUILD_TEST "Build test" ON)
option(MOTO_BUILD_EXAMPLES "Build examples" ON)
option(MOTO_BUILD_DOCS "Build documents" ON)

option(MOTO_BUILD_CORE_DEBUG "Build for lib core debug" OFF)

######################
## base directories ##
######################
set(MOTO_BASE_DIR      ${PROJECT_SOURCE_DIR})
set(MOTO_BUILD_DIR     ${PROJECT_BINARY_DIR})
set(MOTO_BIN_BASE      ${MOTO_BUILD_DIR}/bin)
set(MOTO_3RD_BASE      ${MOTO_BASE_DIR}/3rd)
set(MOTO_INCLUDE_BASE  ${MOTO_BASE_DIR}/include)
set(MOTO_CORE          ${MOTO_INCLUDE_BASE}/core)
set(MOTO_EXT           ${MOTO_INCLUDE_BASE}/ext)
set(MOTO_IO            ${MOTO_INCLUDE_BASE}/io)
set(MOTO_DOC_BASE      ${MOTO_BASE_DIR}/docs)
set(MOTO_DOC_OUT_DIR   ${MOTO_BUILD_DIR}/docs)
set(MOTO_TESTS_BASE    ${MOTO_BASE_DIR}/tests)
set(MOTO_EXAMPLES_BASE ${MOTO_BASE_DIR}/examples)
####################################
## default build options for moto ##
####################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -funroll-loops -DNDEBUG -DMOTO_USE_LOG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -funroll-loops -g3 -DNDEBUG -D_GLIBCXX_DEBUG -DMOTO_USE_LOG -DMOTO_USE_ASSERT")
set(CMAKE_CXX_FLAGS_MEMCHECK "-O3 -g3 -fsanitize=address -DDEBUG -D_GLIBCXX_DEBUG -DMOTO_USE_LOG -DMOTO_USE_ASSERT -DMOTO_USE_TRACE")
set(CMAKE_CXX_FLAGS_DEBUG "-O1 -g3 -ggdb -DDEBUG -D_GLIBCXX_DEBUG -DMOTO_USE_LOG -DMOTO_USE_ASSERT -DMOTO_USE_TRACE")
add_compile_options(
  -finline-functions
  -fno-omit-frame-pointer
  -W -Wall -Wextra -Wctor-dtor-privacy -Wnon-virtual-dtor
  -Wold-style-cast -Woverloaded-virtual -Wreorder
)
####################
## for LSP suport ##
####################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if (MOTO_BUILD_TEST)
  include(CTest)
  set(MOTO_TESTS_OUT_DIR ${MOTO_BIN_BASE}/tests)
  set(MOTO_TESTER_NAME "doctest")
  set(MOTO_TESTER_BASE ${MOTO_3RD_BASE}/tester/${MOTO_TESTER_NAME})
  include_directories(${MOTO_TESTER_BASE})
  add_subdirectory(${MOTO_TESTS_BASE}/core)
  add_subdirectory(${MOTO_TESTS_BASE}/ext)
endif()

if (MOTO_BUILD_EXAMPLES)
  set(MOTO_EXAMPLES_OUT_DIR ${MOTO_BIN_BASE})
  add_subdirectory(${MOTO_EXAMPLES_BASE}/core)
  add_subdirectory(${MOTO_EXAMPLES_BASE}/ext)
  # add_subdirectory(${MOTO_EXAMPLES_BASE})
endif()

if (MOTO_BUILD_DOCS)
  add_subdirectory(${MOTO_DOC_BASE})
endif ()
