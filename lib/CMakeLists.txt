# common config stuff for all flex components
add_library(flex-common-config INTERFACE)
target_include_directories(flex-common-config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(flex-common-config INTERFACE cxx_std_20)


# macros component
add_library(flex-macros INTERFACE)
add_library(flex::macros ALIAS flex-macros)
target_link_libraries(flex-macros INTERFACE
	flex-common-config
	flex-autogen-macros
)
target_compile_definitions(flex-macros INTERFACE
	FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT=${FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT}
	FLEX_MACROS_MAX_INT_SEQ_SIZE=${FLEX_MACROS_MAX_INT_SEQ_SIZE}
	FLEX_MACROS_FOR_LOOP_MAX_DEPTH=${FLEX_MACROS_FOR_LOOP_MAX_DEPTH}
)


# containers component
add_library(flex-containers INTERFACE)
add_library(flex::containers ALIAS flex-containers)
target_link_libraries(flex-containers INTERFACE
	flex-common-config
)


# reflection component
add_library(flex-reflection INTERFACE)
add_library(flex::reflection ALIAS flex-reflection)
target_link_libraries(flex-reflection INTERFACE
	flex-common-config
	flex-macros
	flex-containers
)
target_compile_definitions(flex-reflection INTERFACE
	FLEX_REFLECTION_MAX_MEMBERS_COUNT=${FLEX_REFLECTION_MAX_MEMBERS_COUNT}
)


# enums component
add_library(flex-enums INTERFACE)
add_library(flex::enums ALIAS flex-enums)
target_link_libraries(flex-enums INTERFACE
	flex-common-config
)
target_compile_definitions(flex-enums INTERFACE
	FLEX_ENUMS_MAX_DEFAULT_ENUM_SIZE=${FLEX_ENUMS_MAX_DEFAULT_ENUM_SIZE}
)




# header-only library
#add_library(flex_lib_header_only INTERFACE)
#add_library(flex::flex_header_only ALIAS flex_lib_header_only)
#set_property(TARGET flex_lib_header_only PROPERTY CXX_STANDARD ${FLEX_CPP_DIALECT})
#add_dependencies(flex_lib_header_only flex_lib_autogen)

#target_compile_definitions(flex_lib_header_only INTERFACE
#	FLEX_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
#	FLEX_VERSION_MINOR=${PROJECT_VERSION_MINOR}
#	FLEX_VERSION_PATCH=${PROJECT_VERSION_PATCH}
#	FLEX_REFLECTION_MAX_ENUM_SIZE=${FLEX_REFLECTION_MAX_ENUM_SIZE}
#)

#target_include_directories(flex_lib_header_only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${FLEX_LIBRARY_AUTOGEN_INCLUDE_DIR})

#if (PROJECT_IS_TOP_LEVEL)
#	if (MSVC)
#		target_compile_options(flex_lib_header_only INTERFACE /W4)
#	else()
#		target_compile_options(flex_lib_header_only INTERFACE -Wall -Wextra -Wpedantic)
#	endif()
#endif()


# library itself
#if (NOT FLEX_BUILD_HEADER_ONLY)
#	file(GLOB_RECURSE FLEX_LIBRARY_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp)
#	file(GLOB_RECURSE FLEX_LIBRARY_INLINE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)
#	file(GLOB_RECURSE FLEX_LIBRARY_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

#	add_library(flex_lib STATIC ${FLEX_LIBRARY_HEADER} ${FLEX_LIBRARY_INLINE} ${FLEX_LIBRARY_SOURCE})
#	add_library(flex::flex ALIAS flex_lib)
#	set_property(TARGET flex_lib PROPERTY CXX_STANDARD ${FLEX_CPP_DIALECT})
#	target_link_libraries(flex_lib PUBLIC flex::flex_header_only)
#endif()

