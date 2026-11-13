# Moto

The name **Moto** comes from the Japanese Kanji *"基"*, meaning the "origin" or "base".

**Moto**: **M**odular **O**rigin for **To**ols  A header-only C++20 library that provides the essential building blocks for embedded frameworks, like the motor that drives your project forward.

## Getting and building

``` cmd
user@some-dir$ git clone --recurse-submodules https://codeberg.org/s3mat3/moto.git

      OR

user@some-dir$ git clone --recurse-submodules https://github.com/s3mat3/moto.git
```


### Building tests

Useing doctest(https://github.com/doctest/doctest.git) (including as submodule)

``` cmd
user@moto$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Memcheck -DMOTO_BUILD_TEST=On
user@moto$ cmake --build build
```


### Building examples

``` cmd
user@moto$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Memcheck -DMOTO_BUILD_EXAMPLES=On
user@moto$ cmake --build build
```

### Building API document

Depends on doxygen.

``` cmd
user@moto$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Memcheck -DMOTO_BUILD_DOCS=On
user@moto$ cmake --build build
```

## Overview of files

### core

- **event_dispatcher.hpp**:
  - A thread-safe event dispatcher for managing and notifying listeners.
- **debug.hpp**:
  - Debugging utilities for type demangling, variable dumping, and assertions.
- **logger.hpp**:
  - Logging utility providing leveled output with source location tracking.
- **result.hpp**:
  - A return value type class aimed at something similar to the return value of the Go language.
- **timer.hpp**:
  - A thread-safe timer class for measuring elapsed time and handling timeouts.

### ext

- **event_bus.hpp**:
  - Event bus implementation for decoupled communication.
- **fsm.hpp**:
  - Finite State Machine (FSM) implementation.
- **thread_pool.hpp**:
  - A robust thread pool implementation using Result for error handling.
