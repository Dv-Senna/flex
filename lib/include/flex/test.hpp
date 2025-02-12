#pragma once

#include <iostream>


namespace flex {
	inline void sayHello() {
		#if defined(_WIN32)
			std::cout << "Hello from Windows testlib !" << std::endl;
		#elif defined(__linux__)
			std::cout << "Hello from Linux testlib !" << std::endl;
		#elif defined(__APPLE__)
			std::cout << "Hello from Apple testlib !" << std::endl;
		#endif

		#if defined(__clang__)
			std::cout << "Using clang as a compiler" << std::endl;
		#elif defined(__GNUC__) || defined(__GNUG__)
			std::cout << "Using gcc as a compiler" << std::endl;
		#elif defined(_MSC_VER)
			std::cout << "Using MSVC as a compiler" << std::endl;
		#endif
	}
}
