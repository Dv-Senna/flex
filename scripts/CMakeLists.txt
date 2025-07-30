if (NOT Python3_Interpreter_FOUND)
	message("Flex: Python3 interpreter not found. Some components of flex may not be usable")

	# adding library as empty to avoid issue with cmake not finding them
	add_library(flex-autogen-macros INTERFACE)
	return()
endif()

add_custom_target(flex-generate-macros-var-to-seq
	COMMAND
		Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/macros_var_to_seq.py
		${CMAKE_CURRENT_BINARY_DIR}/macros/include/flex/macros/autogen/varToSeq.hpp
		${FLEX_MACROS_MAX_VAR_TO_SEQ_COUNT}
	DEPENDS
		${CMAKE_CURRENT_SOURCE_DIR}/macros_var_to_seq.py
)


add_library(flex-autogen-macros INTERFACE)
target_include_directories(flex-autogen-macros INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/macros/include)
add_dependencies(flex-autogen-macros
	flex-generate-macros-var-to-seq
)
