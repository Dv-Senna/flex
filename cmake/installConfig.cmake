include(CMakePackageConfigHelpers)

set(FLEX_INCLUDE_DIR_SUFFIX include/flex/v${PROJECT_VERSION_MAJOR})

function(flex_install_component COMPONENT)
	cmake_parse_arguments(ARG
		""
		"TARGET"
		"SUBTARGETS"
		${ARGN}
	)
	if (ARG_TARGET)
		set(TARGET ${ARG_TARGET})
	else()
		set(TARGET flex-${COMPONENT})
	endif()
	set(INCLUDE_DIR_SUFFIX ${FLEX_INCLUDE_DIR_SUFFIX}/${COMPONENT})
	string(TOUPPER ${COMPONENT} UPPER_COMPONENT)

	file(GLOB_RECURSE HPP_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/flex/${COMPONENT}/*.hpp)
	file(GLOB_RECURSE INL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/flex/${COMPONENT}/*.inl)

	target_include_directories(${TARGET}
		INTERFACE
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INCLUDE_DIR_SUFFIX}>
	)
	install(TARGETS ${TARGET} ${ARG_SUBTARGETS} EXPORT flex-targets
		COMPONENT ${UPPER_COMPONENT}
		INCLUDES DESTINATION ${INCLUDE_DIR_SUFFIX}
	)
	install(
		FILES
			${HPP_HEADERS}
			${INL_HEADERS}
		DESTINATION ${INCLUDE_DIR_SUFFIX}/flex/${COMPONENT}
		COMPONENT ${UPPER_COMPONENT}
	)
endfunction()


function(flex_install)
	install(EXPORT flex-targets
		FILE flexTargets.cmake
		NAMESPACE flex::
		DESTINATION lib/cmake/flex
	)
	write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/flexConfigVersion.cmake"
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMinorVersion
	)
	install(
		FILES
			"${PROJECT_SOURCE_DIR}/cmake/flexConfig.cmake"
			"${CMAKE_CURRENT_BINARY_DIR}/flexConfigVersion.cmake"
		DESTINATION lib/cmake/flex
	)
endfunction()
