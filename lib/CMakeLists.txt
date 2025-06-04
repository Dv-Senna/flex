cmake_minimum_required(VERSION 3.21)


# find python3 interpreter
find_package(Python3 COMPONENTS Interpreter)
if (NOT ${Python3_FOUND})
	message(FATAL_ERROR "You need a python3 interpreter to be able to build this project")
endif()

# autogen files config
set(FLEX_LIBRARY_SCRIPTS_DIR ${PROJECT_SOURCE_DIR}/scripts)
set(FLEX_LIBRARY_AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

set(FLEX_LIBRARY_AGGREGATE_MEMBERS_TUPLE_SCRIPTS ${FLEX_LIBRARY_SCRIPTS_DIR}/aggregate_members_tuple.py)
set(FLEX_LIBRARY_AGGREGATE_MEMBERS_TUPLE_HEADER ${FLEX_LIBRARY_AUTOGEN_INCLUDE_DIR}/flex/reflection/autogen/aggregateMembersTuple.hpp)

set(FLEX_LIBRARY_MACROS_VAR_TO_SEQ_SCRIPTS ${FLEX_LIBRARY_SCRIPTS_DIR}/macros_var_to_seq.py)
set(FLEX_LIBRARY_MACROS_VAR_TO_SEQ_HEADER ${FLEX_LIBRARY_AUTOGEN_INCLUDE_DIR}/flex/macros/autogen/varToSeq.hpp)

add_custom_target(flex_lib_autogen ALL DEPENDS
	${FLEX_LIBRARY_AGGREGATE_MEMBERS_TUPLE_HEADER}
	${FLEX_LIBRARY_MACROS_VAR_TO_SEQ_HEADER}
)

# aggregate members tuple
add_custom_command(
	OUTPUT ${FLEX_LIBRARY_AGGREGATE_MEMBERS_TUPLE_HEADER}
	COMMAND ${Python3_EXECUTABLE} ${FLEX_LIBRARY_AGGREGATE_MEMBERS_TUPLE_SCRIPTS} ${FLEX_LIBRARY_AGGREGATE_MEMBERS_TUPLE_HEADER} ${FLEX_REFLECTION_MAX_MEMBERS_COUNT}
	DEPENDS ${FLEX_LIBRARY_AGGREGATE_MEMBERS_TUPLE_SCRIPTS}
)

# macros var to seq
add_custom_command(
	OUTPUT ${FLEX_LIBRARY_MACROS_VAR_TO_SEQ_HEADER}
	COMMAND ${Python3_EXECUTABLE} ${FLEX_LIBRARY_MACROS_VAR_TO_SEQ_SCRIPTS} ${FLEX_LIBRARY_MACROS_VAR_TO_SEQ_HEADER} ${FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT}
	DEPENDS ${FLEX_LIBRARY_MACROS_VAR_TO_SEQ_SCRIPTS}
)


# header-only library
add_library(flex_lib_header_only INTERFACE)
add_library(flex::flex_header_only ALIAS flex_lib_header_only)
set_property(TARGET flex_lib_header_only PROPERTY CXX_STANDARD ${FLEX_CPP_DIALECT})
add_dependencies(flex_lib_header_only flex_lib_autogen)

target_compile_definitions(flex_lib_header_only INTERFACE
	FLEX_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
	FLEX_VERSION_MINOR=${PROJECT_VERSION_MINOR}
	FLEX_VERSION_PATCH=${PROJECT_VERSION_PATCH}
	FLEX_REFLECTION_MAX_ENUM_SIZE=${FLEX_REFLECTION_MAX_ENUM_SIZE}
)

target_include_directories(flex_lib_header_only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${FLEX_LIBRARY_AUTOGEN_INCLUDE_DIR})

if (PROJECT_IS_TOP_LEVEL)
	if (MSVC)
		target_compile_options(flex_lib_header_only INTERFACE /W4)
	else()
		target_compile_options(flex_lib_header_only INTERFACE -Wall -Wextra -Wpedantic)
	endif()
endif()


# library itself
if (NOT FLEX_BUILD_HEADER_ONLY)
	file(GLOB_RECURSE FLEX_LIBRARY_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp)
	file(GLOB_RECURSE FLEX_LIBRARY_INLINE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)
	file(GLOB_RECURSE FLEX_LIBRARY_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

	add_library(flex_lib STATIC ${FLEX_LIBRARY_HEADER} ${FLEX_LIBRARY_INLINE} ${FLEX_LIBRARY_SOURCE})
	add_library(flex::flex ALIAS flex_lib)
	set_property(TARGET flex_lib PROPERTY CXX_STANDARD ${FLEX_CPP_DIALECT})
	target_link_libraries(flex_lib PUBLIC flex::flex_header_only)
endif()

