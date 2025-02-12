#pragma once

#include "flex/basicTypes.hpp"


namespace flex::config {
	constexpr flex::Version VERSION {__FLEX_VERSION_MAJOR, __FLEX_VERSION_MINOR, __FLEX_VERSION_PATCH};

#ifdef __linux__
	constexpr flex::OS OS {flex::OS::eLinux};
#elifdef _WIN32
	constexpr flex::OS OS {flex::OS::eWindows};
#elifdef __APPLE__
	constexpr flex::OS OS {flex::OS::eMacos};
#else
	#error "You're using an unsupported OS"
	#include <stop_compilation>
#endif

#ifdef __clang__
	constexpr flex::Compiler COMPILER {flex::Compiler::eClang};
#elifdef __GNUC__
	constexpr flex::Compiler COMPILER {flex::Compiler::eGCC};
#elifdef _MSC_VER
	constexpr flex::Compiler COMPILER {flex::Compiler::eMSVC};
#else
	#error "You're using an unsupported compiler"
#endif

#ifdef NDEBUG
	constexpr flex::BuildType BUILD_TYPE {flex::BuildType::eRelease};
#else
	constexpr flex::BuildType BUILD_TYPE {flex::BuildType::eDebug};
#endif

} // namespace flex::config
