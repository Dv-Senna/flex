#pragma once

#include <iostream>

#include "flex/config.hpp"


namespace flex {
	inline void sayHello() {
		if constexpr (flex::config::OS == flex::OS::eWindows)
			std::cout << "Hello from Windows flex !" << std::endl;
		else if constexpr (flex::config::OS == flex::OS::eLinux)
			std::cout << "Hello from Linux flex !" << std::endl;
		else if constexpr (flex::config::OS == flex::OS::eMacos)
			std::cout << "Hello from Apple flex !" << std::endl;

		if constexpr (flex::config::COMPILER == flex::Compiler::eClang)
			std::cout << "Using clang as a compiler" << std::endl;
		else if constexpr (flex::config::COMPILER == flex::Compiler::eGCC)
			std::cout << "Using gcc as a compiler" << std::endl;
		else if constexpr (flex::config::COMPILER == flex::Compiler::eMSVC)
			std::cout << "Using MSVC as a compiler" << std::endl;
	}
}
