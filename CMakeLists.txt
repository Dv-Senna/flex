cmake_minimum_required(VERSION 3.21)

project(flex
	VERSION 0.0.0
	LANGUAGES CXX
)

# config
if (NOT DEFINED FLEX_CPP_DIALECT)
	set(FLEX_CPP_DIALECT 23)
endif()

option(FLEX_BUILD_ONLY_HEADER OFF)

if (${PROJECT_IS_TOP_LEVEL})
	set(FLEX_DEFAULT_BUILD_TESTS On)
else()
	set(FLEX_DEFAULT_BUILD_TESTS Off)
endif()


# options
option(FLEX_BUILD_TESTS "FLEX_BUILD_TESTS" ${FLEX_DEFAULT_BUILD_TESTS})
set(FLEX_REFLECTION_MAX_MEMBERS_COUNT 128)
set(FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT 128)
set(FLEX_REFLECTION_MAX_ENUM_SIZE 32)


# subdirectories
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
if (FLEX_BUILD_TESTS)
	add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()
