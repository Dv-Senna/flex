#pragma once


#ifdef __cpp_exceptions
	#define FLEX_TRY try
	#define FLEX_CATCH(...) catch (__VA_ARGS__)
	#define FLEX_THROW(...) throw __VA_ARGS__
#else
	#define FLEX_TRY if (true)
	#define FLEX_CATCH(...) if (false)
	#define FLEX_THROW(...) std::abort()
#endif
