cmake_minimum_required(VERSION 3.20)

project(flex
	VERSION 0.0.0
	LANGUAGES CXX
)

# dependencies
find_package(Python3 COMPONENTS Interpreter)

# options
set(FLEX_REFLECTION_MAX_MEMBERS_COUNT 128 CACHE STRING "")
set(FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT  128 CACHE STRING "")
set(FLEX_MACROS_MAX_INT_SEQ_SIZE      128 CACHE STRING "")


# subdirectories
add_subdirectory(${PROJECT_SOURCE_DIR}/scripts EXCLUDE_FROM_ALL)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests EXCLUDE_FROM_ALL)
