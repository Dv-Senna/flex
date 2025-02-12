cmake_minimum_required(VERSION 3.21)


add_library(flex_interface INTERFACE)
add_library(flex::flex ALIAS flex_interface)
set_property(TARGET flex_interface PROPERTY CXX_STANDARD ${FIPPER_CPP_DIALECT})

target_compile_definitions(flex_interface INTERFACE
	__FLEX_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
	__FLEX_VERSION_MINOR=${PROJECT_VERSION_MINOR}
	__FLEX_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

target_include_directories(flex_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
