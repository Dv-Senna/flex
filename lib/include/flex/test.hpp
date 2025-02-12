#pragma once

#include <print>

#include "flex/config.hpp"


namespace flex {
	inline void sayHello() {
		if constexpr (flex::config::OS == flex::OS::eWindows)
			std::println("Hello from Windows flex !");
		else if constexpr (flex::config::OS == flex::OS::eLinux)
			std::println("Hello from Linux flex !");
		else if constexpr (flex::config::OS == flex::OS::eMacos)
			std::println("Hello from Apple flex !");

		if constexpr (flex::config::COMPILER == flex::Compiler::eClang)
			std::println("Using clang as a compiler");
		else if constexpr (flex::config::COMPILER == flex::Compiler::eGCC)
			std::println("Using gcc as a compiler");
		else if constexpr (flex::config::COMPILER == flex::Compiler::eMSVC)
			std::println("Using MSVC as a compiler");

		if constexpr (flex::config::BUILD_TYPE == flex::BuildType::eDebug)
			std::println("Debug build");
		else if constexpr (flex::config::BUILD_TYPE == flex::BuildType::eRelease)
			std::println("Release build");
	}
}
