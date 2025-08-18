cmake_minimum_required(VERSION 3.20)

project(flex
	VERSION 1.0.0
	LANGUAGES CXX
)


# dependencies
find_package(Python3 COMPONENTS Interpreter)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Catch2.cmake)

# options
set(FLEX_REFLECTION_MAX_MEMBERS_COUNT 128 CACHE STRING "")
set(FLEX_ENUMS_MAX_DEFAULT_ENUM_SIZE  16 CACHE STRING "")
set(FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT  128 CACHE STRING "")
set(FLEX_MACROS_MAX_INT_SEQ_SIZE      128 CACHE STRING "")
set(FLEX_MACROS_FOR_LOOP_MAX_DEPTH    16  CACHE STRING "")


# subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/scripts)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests EXCLUDE_FROM_ALL)
