include(CMakeParseArguments)

file(GLOB_RECURSE FLEX_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(TEST_TARGETS)

function(add_test TARGET)
	cmake_parse_arguments(ARG
		""
		"STANDARD"
		"MODULES"
		${ARGN}
	)

	set(TARGET_NAME flex-tests-${TARGET}-${ARG_STANDARD})
	list(TRANSFORM ARG_MODULES PREPEND "flex::")
	list(APPEND TEST_TARGETS ${TARGET_NAME})
	set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)

	add_executable(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET}.cpp)
	target_link_libraries(${TARGET_NAME} PRIVATE ${ARG_MODULES})
	target_compile_features(${TARGET_NAME} PRIVATE cxx_std_${ARG_STANDARD})

	if (MSVC)
		target_compile_options(${TARGET_NAME} PRIVATE /W4)
	else()
		target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic)
	endif()
endfunction()

function(add_module_test MODULE)
	cmake_parse_arguments(ARG "" "" "STANDARDS" ${ARGN})
	if (NOT DEFINED ARG_STANDARDS)
		set(ARG_STANDARDS 20)
	endif()
	list(LENGTH ARG_STANDARDS STANDARDS_LENGTH)
	if (STANDARDS_LENGTH LESS 1)
		message(FATAL_ERROR "You must provide at least one C++ standard")
	endif()

	foreach (STANDARD IN LISTS ARG_STANDARDS)
		add_test(${MODULE}
			STANDARD
				${STANDARD}
			MODULES
				${MODULE}
		)
	endforeach()
	set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)
endfunction()



add_module_test(macros STANDARDS 23)
add_module_test(reflection STANDARDS 20 23)


add_custom_target(flex-tests
	DEPENDS
		${TEST_TARGETS}
)
