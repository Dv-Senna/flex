#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)

#ifdef __cpp_exceptions
	#define FLEX_TRY try
	#define FLEX_CATCH(...) catch (__VA_ARGS__)
	#define FLEX_THROW(...) throw __VA_ARGS__
#else
	#define FLEX_TRY if constexpr (true)
	#define FLEX_CATCH(...) if constexpr (false)
	#define FLEX_THROW(...) std::abort()
#endif

#define FLEX_CPP_20 202002L
#define FLEX_CPP_23 202302L
#define FLEX_CPP_26 202400L // temporary value used by current C++26 compiler

// NOLINTEND(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)
