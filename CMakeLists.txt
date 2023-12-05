cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

# Set project information
project(LibWebVTT VERSION 1.0.0 LANGUAGES C CXX)
set(PROJECT_BRIEF "A cross-platform implementation of a WebVTT parser written in C and C++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (NOT MSVC)
  SET(GCC_COVERAGE_COMPILE_FLAGS "-Wno-narrowing")
endif (NOT MSVC)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_definitions(-DWEBVTT_BUILD_LIBRARY)
set(-DWEBVTT_BUILD_LIBRARY 0)
if (BUILD_LIBRARY)
  set(-DWEBVTT_BUILD_LIBRARY 1)
endif (BUILD_LIBRARY)

add_subdirectory("src")

if (MSVC)
  # On Windows gtest uses Static CRT by default. This option fixes linking problems.
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif (MSVC)

add_subdirectory("test")
