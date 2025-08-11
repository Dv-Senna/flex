include(CMakeParseArguments)
include(${PROJECT_SOURCE_DIR}/cmake/installConfig.cmake)

if (NOT Python3_Interpreter_FOUND)
	message("Flex: Python3 interpreter not found. Some components of flex may not be usable")

	# adding library as empty to avoid issue with cmake not finding them
	add_library(flex-autogen-macros INTERFACE)
	return()
endif()

function(add_python_script)
	cmake_parse_arguments(ARG
		""
		"OUTPUT;SCRIPT;VARIABLE;COMPONENT"
		""
		${ARGN}
	)
	add_custom_command(
		OUTPUT
			${ARG_OUTPUT}
		COMMAND
			Python3::Interpreter ${ARG_SCRIPT} ${ARG_OUTPUT} ${ARG_VARIABLE}
		DEPENDS
			${ARG_SCRIPT}
	)

	string(TOUPPER ${ARG_COMPONENT} UPPER_COMPONENT)
	install(
		FILES
			${ARG_OUTPUT}
		DESTINATION
			${FLEX_INCLUDE_DIR_SUFFIX}/${ARG_COMPONENT}/flex/${ARG_COMPONENT}/autogen
		COMPONENT
			${UPPER_COMPONENT}
	)
endfunction()

add_python_script(
	SCRIPT
		${CMAKE_CURRENT_SOURCE_DIR}/macros_var_to_seq.py
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/varToSeq.hpp
	VARIABLE
		${FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT}
	COMPONENT
		macros
)
add_python_script(
	SCRIPT
		${CMAKE_CURRENT_SOURCE_DIR}/macros_int_seq.py
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/intSeq.hpp
	VARIABLE
		${FLEX_MACROS_MAX_INT_SEQ_SIZE}
	COMPONENT
		macros
)
add_python_script(
	SCRIPT
		${CMAKE_CURRENT_SOURCE_DIR}/macros_increment.py
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/increment.hpp
	VARIABLE
		${FLEX_MACROS_MAX_INT_SEQ_SIZE}
	COMPONENT
		macros
)
add_python_script(
	SCRIPT
		${CMAKE_CURRENT_SOURCE_DIR}/macros_foreach_depth.py
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/foreachDepth.hpp
	VARIABLE
		${FLEX_MACROS_FOR_LOOP_MAX_DEPTH}
	COMPONENT
		macros
)
add_python_script(
	SCRIPT
		${CMAKE_CURRENT_SOURCE_DIR}/macros_iota_for_depth.py
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/iotaForDepth.hpp
	VARIABLE
		${FLEX_MACROS_FOR_LOOP_MAX_DEPTH}
	COMPONENT
		macros
)


add_custom_target(flex-generate-macros
	DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/varToSeq.hpp
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/intSeq.hpp
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/increment.hpp
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/foreachDepth.hpp
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/iotaForDepth.hpp
)

add_library(flex-autogen-macros INTERFACE EXCLUDE_FROM_ALL)
target_include_directories(flex-autogen-macros INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/macros/include>)
add_dependencies(flex-autogen-macros
	flex-generate-macros
)
