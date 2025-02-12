cmake_minimum_required(VERSION 3.21)


file(GLOB_RECURSE FLEX_LIBRARY_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp)
file(GLOB_RECURSE FLEX_LIBRARY_INLINE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)

add_library(flex_interface INTERFACE ${FLEX_LIBRARY_HEADER} ${FLEX_LIBRARY_INLINE})
add_library(flex::flex ALIAS flex_interface)
set_property(TARGET flex_interface PROPERTY CXX_STANDARD ${FIPPER_CPP_DIALECT})


target_compile_definitions(flex_interface INTERFACE
	__FLEX_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
	__FLEX_VERSION_MINOR=${PROJECT_VERSION_MINOR}
	__FLEX_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

target_include_directories(flex_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
