#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendors/Catch2)

file(GLOB_RECURSE FLEX_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(TEST_TARGETS)

foreach (FLEX_TEST_SOURCE_FILE ${FLEX_TEST_SOURCES})
	get_filename_component(FLEX_TEST_FILENAME ${FLEX_TEST_SOURCE_FILE} NAME_WE)
	set(FLEX_TEST_EXE_NAME flex_test_${FLEX_TEST_FILENAME})
	list(APPEND TEST_TARGETS ${FLEX_TEST_EXE_NAME})

	add_executable(${FLEX_TEST_EXE_NAME} ${FLEX_TEST_SOURCE_FILE})
	target_link_libraries(${FLEX_TEST_EXE_NAME} PRIVATE flex::macros)
	target_compile_features(${FLEX_TEST_EXE_NAME} PRIVATE cxx_std_23)
#	target_link_libraries(${FLEX_TEST_EXE_NAME} PRIVATE flex::flex Catch2::Catch2WithMain)

	if (MSVC)
		target_compile_options(${FLEX_TEST_EXE_NAME} PRIVATE /W4)
	else()
		target_compile_options(${FLEX_TEST_EXE_NAME} PRIVATE -Wall -Wextra -pedantic)
	endif()
endforeach()

add_custom_target(flex-tests
	DEPENDS
		${TEST_TARGETS}
)
