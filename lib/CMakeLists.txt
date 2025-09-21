include(CMakeParseArguments)
include(${PROJECT_SOURCE_DIR}/cmake/installConfig.cmake)


# common config stuff for all flex components
add_library(flex-common-config INTERFACE)
target_include_directories(flex-common-config INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(flex-common-config INTERFACE cxx_std_20)
flex_install_component(core TARGET flex-common-config SUBTARGETS flex-autogen-macros)


# macros component
add_library(flex-macros INTERFACE)
add_library(flex::flex-macros ALIAS flex-macros)
target_link_libraries(flex-macros INTERFACE
	flex-common-config
	flex-autogen-macros
)
target_compile_definitions(flex-macros INTERFACE
	FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT=${FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT}
	FLEX_MACROS_MAX_INT_SEQ_SIZE=${FLEX_MACROS_MAX_INT_SEQ_SIZE}
	FLEX_MACROS_FOR_LOOP_MAX_DEPTH=${FLEX_MACROS_FOR_LOOP_MAX_DEPTH}
)
flex_install_component(macros)


# containers component
add_library(flex-containers INTERFACE)
add_library(flex::flex-containers ALIAS flex-containers)
target_link_libraries(flex-containers INTERFACE
	flex-common-config
)
flex_install_component(containers)


# reflection component
add_library(flex-reflection INTERFACE)
add_library(flex::flex-reflection ALIAS flex-reflection)
target_link_libraries(flex-reflection INTERFACE
	flex-common-config
	flex-macros
	flex-containers
)
target_compile_definitions(flex-reflection INTERFACE
	FLEX_REFLECTION_MAX_MEMBERS_COUNT=${FLEX_REFLECTION_MAX_MEMBERS_COUNT}
)
flex_install_component(reflection)


# enums component
add_library(flex-enums INTERFACE)
add_library(flex::flex-enums ALIAS flex-enums)
target_link_libraries(flex-enums INTERFACE
	flex-common-config
)
target_compile_definitions(flex-enums INTERFACE
	FLEX_ENUMS_MAX_DEFAULT_ENUM_SIZE=${FLEX_ENUMS_MAX_DEFAULT_ENUM_SIZE}
)
flex_install_component(enums)


# pipes component
add_library(flex-pipes INTERFACE)
add_library(flex::flex-pipes ALIAS flex-pipes)
target_link_libraries(flex-pipes INTERFACE
	flex-common-config
)
flex_install_component(pipes)


flex_install()
