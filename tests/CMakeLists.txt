cmake_minimum_required(VERSION 3.21)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendors/Catch2)

file(GLOB_RECURSE FLEX_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

foreach (FLEX_TEST_SOURCE_FILE ${FLEX_TEST_SOURCES})
	get_filename_component(FLEX_TEST_FILENAME ${FLEX_TEST_SOURCE_FILE} NAME_WE)
	set(FLEX_TEST_EXE_NAME flex_test_${FLEX_TEST_FILENAME})

	add_executable(${FLEX_TEST_EXE_NAME} ${FLEX_TEST_SOURCE_FILE})
	set_property(TARGET ${FLEX_TEST_EXE_NAME} PROPERTY CXX_STANDARD ${FLEX_CPP_DIALECT})

	target_link_libraries(${FLEX_TEST_EXE_NAME} PRIVATE flex::flex Catch2::Catch2WithMain)

	if (MSVC)
		target_compile_options(${FLEX_TEST_EXE_NAME} PRIVATE /W4)
	else()
		target_compile_options(${FLEX_TEST_EXE_NAME} PRIVATE -Wall -Wextra -pedantic)
	endif()
endforeach()

