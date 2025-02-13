#pragma once

#if __cplusplus < 201703L
	#error "Flex requires at least C++17"
	#include <stop_compilation>
#endif

#if __cplusplus >= 202002L
	#include <version>
#endif

#include "flex/basicTypes.hpp"


namespace flex::config {
	constexpr flex::Version VERSION {__FLEX_VERSION_MAJOR, __FLEX_VERSION_MINOR, __FLEX_VERSION_PATCH};

#if defined(__linux__)
	constexpr flex::OS OS {flex::OS::eLinux};
	#define FLEX_CONFIG_OS_LINUX
#elif defined(_WIN32)
	constexpr flex::OS OS {flex::OS::eWindows};
	#define FLEX_CONFIG_OS_WINDOWS
#elif defined(__APPLE__)
	constexpr flex::OS OS {flex::OS::eMacos};
	#define FLEX_CONFIG_OS_APPLE
#else
	#error "You're using an unsupported OS"
	#include <stop_compilation>
#endif

#if defined(__clang__)
	constexpr flex::Compiler COMPILER {flex::Compiler::eClang};
	#define FLEX_CONFIG_COMPILER_CLANG
#elif defined(__GNUC__)
	constexpr flex::Compiler COMPILER {flex::Compiler::eGCC};
	#define FLEX_CONFIG_COMPILER_GCC
#elif defined(_MSC_VER)
	constexpr flex::Compiler COMPILER {flex::Compiler::eMSVC};
	#define FLEX_CONFIG_COMPILER_MSVC
#else
	#error "You're using an unsupported compiler"
	#include <stop_compilation>
#endif

#if defined(NDEBUG)
	constexpr flex::BuildType *BUILD_TYPE {flex::BuildType::eRelease};
	#define FLEX_BUILD_TYPE_RELEASE
#else
	constexpr flex::BuildType BUILD_TYPE {flex::BuildType::eDebug};
	#define FLEX_BUILD_TYPE_DEBUG
#endif

#if __cplusplus >= 202302L
	constexpr flex::CppStandard CPP_STANDARD {flex::CppStandard::e23};
	#define FLEX_CONFIG_CPP_STANDARD 23
#elif __cplusplus >= 202002L
	constexpr flex::CppStandard CPP_STANDARD {flex::CppStandard::e20};
	#define FLEX_CONFIG_CPP_STANDARD 20
#elif __cplusplus >= 201703L
	constexpr flex::CppStandard CPP_STANDARD {flex::CppStandard::e17};
	#define FLEX_CONFIG_CPP_STANDARD 17
#else
	#error "You're using an unknown / unsupported / invalid version of the standard"
	#include <stop_compilation>
#endif

	namespace cppFeatures {
	#if defined(__cpp_concepts)
		constexpr bool CONCEPTS {true};
		#define FLEX_CONFIG_CPP_FEATURES_CONCEPTS
	#else
		constexpr bool CONCEPTS {false};
	#endif

	#if defined(__cpp_consteval)
		constexpr bool CONSTEVAL {true};
		#define FLEX_CONSTEVAL consteval
		#define FLEX_CONFIG_CPP_FEATURES_CONSTEVAL
	#else
		constexpr bool CONSTEVAL {false};
		#define FLEX_CONSTEVAL constexpr
	#endif

	#if defined(__cpp_lib_format)
		constexpr bool LIB_FORMAT {true};
		#define FLEX_CONFIG_CPP_FEATURES_LIB_FORMAT
	#else
		constexpr bool LIB_FORMAT {false};
	#endif

	#if defined(__cpp_lib_print)
		constexpr bool LIB_PRINT {true};
		#define FLEX_CONFIG_CPP_FEATURES_LIB_PRINT
	#else
		constexpr bool LIB_PRINT {false};
	#endif

	#if defined(__cpp_lib_ranges)
		constexpr bool LIB_RANGES {true};
		#define FLEX_CONFIG_CPP_FEATURES_LIB_RANGES
	#else
		constexpr bool LIB_RANGES {false};
	#endif

	#if FLEX_CONFIG_CPP_STANDARD >= 20 && __has_cpp_attribute(no_unique_address)
		#define FLEX_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#else
		#define FLEX_NO_UNIQUE_ADDRESS
	#endif

	#if FLEX_CONFIG_CPP_STANDARD >= 20 && __has_cpp_attribute(likely)
		#define FLEX_LIKELY [[likely]]
	#else
		#define FLEX_LIKELY
	#endif

	#if FLEX_CONFIG_CPP_STANDARD >= 20 && __has_cpp_attribute(unlikely)
		#define FLEX_UNLIKELY [[unlikely]]
	#else
		#define FLEX_UNLIKELY
	#endif

	} // namespace cppFeatures

} // namespace flex::config
