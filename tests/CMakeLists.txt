include(CMakeParseArguments)
include(CTest)
include(Catch)

file(GLOB_RECURSE FLEX_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(TEST_TARGETS)

function(add_test TARGET)
	cmake_parse_arguments(ARG
		"COMPTIME_ONLY"
		"STANDARD"
		"MODULES"
		${ARGN}
	)

	set(TARGET_NAME flex-tests-${TARGET}-${ARG_STANDARD})
	list(TRANSFORM ARG_MODULES PREPEND "flex::flex-")
	list(APPEND TEST_TARGETS ${TARGET_NAME})
	set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)

	if (NOT ARG_COMPTIME_ONLY)
		add_executable(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET}.cpp)
	else()
		add_library(${TARGET_NAME} STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET}.cpp)
	endif()
	target_link_libraries(${TARGET_NAME} PRIVATE ${ARG_MODULES})
	target_compile_features(${TARGET_NAME} PRIVATE cxx_std_${ARG_STANDARD})
	if (NOT ARG_COMPTIME_ONLY)
		catch_discover_tests(${TARGET_NAME})
		target_link_libraries(${TARGET_NAME} PRIVATE Catch2::Catch2WithMain)
	endif()

	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -fconcepts-diagnostics-depth=2)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		target_compile_options(${TARGET_NAME} PRIVATE /W4)
	endif()
endfunction()

function(add_module_test MODULE)
	cmake_parse_arguments(ARG "COMPTIME_ONLY" "" "STANDARDS" ${ARGN})
	if (NOT DEFINED ARG_STANDARDS)
		set(ARG_STANDARDS 20)
	endif()
	list(LENGTH ARG_STANDARDS STANDARDS_LENGTH)
	if (STANDARDS_LENGTH LESS 1)
		message(FATAL_ERROR "You must provide at least one C++ standard")
	endif()
	if (ARG_COMPTIME_ONLY)
		set(COMPTIME_ONLY "COMPTIME_ONLY")
	else()
		set(COMPTIME_ONLY "")
	endif()

	foreach (STANDARD IN LISTS ARG_STANDARDS)
		add_test(${MODULE}
			STANDARD
				${STANDARD}
			MODULES
				${MODULE}
			${COMPTIME_ONLY}
		)
	endforeach()
	set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)
endfunction()



add_module_test(macros STANDARDS 23 COMPTIME_ONLY)
add_module_test(reflection STANDARDS 20 23)
add_module_test(enums STANDARDS 20 23)
add_module_test(containers STANDARDS 20 23)


add_custom_target(flex-tests
	DEPENDS
		${TEST_TARGETS}
)
