#pragma once

#include "flex/config.hpp"

#include <type_traits>


namespace flex {
	struct Empty {};

	template <typename T>
	struct make_void {
		using type = void;
	};

	template <typename T>
	using make_void_t = typename make_void<T>::type;


	template <typename T, typename = void>
	struct is_equality_comparable : std::false_type {};

	template <typename T>
	struct is_equality_comparable<T,
		std::void_t<decltype(std::declval<T> () == std::declval<T> ())>
	> : std::true_type {};

	template <typename T>
	constexpr auto is_equality_comparable_v {is_equality_comparable<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept equality_comparable = is_equality_comparable_v<T> && requires(const T val) {
		{val == val};
	};
#endif // concepts

	template <typename T>
	constexpr auto true_v {std::true_type::value};

	template <typename T>
	constexpr auto false_v {std::true_type::value};

} // namespace flex
