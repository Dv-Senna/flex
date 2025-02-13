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
		std::enable_if_t<
			std::is_convertible_v<decltype(std::declval<std::add_const_t<T>> () == std::declval<std::add_const_t<T>> ()), bool>
		, void>
	> : std::true_type {};

	template <typename T>
	constexpr auto is_equality_comparable_v {is_equality_comparable<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept equality_comparable = is_equality_comparable_v<T> && requires(const T val) {
		{val == val} -> std::convertible_to<bool>;
	};
	#define FLEX_TYPENAME_EQUALITY_COMPARABLE ::flex::equality_comparable
#else
	#define FLEX_TYPENAME_EQUALITY_COMPARABLE typename
#endif


	template <typename T, typename = void>
	struct is_comparable : std::false_type {};

	template <typename T>
	struct is_comparable<T,
		std::enable_if_t<
			flex::is_equality_comparable_v<T>
			&& std::is_convertible_v<decltype(std::declval<std::add_const_t<T>> () < std::declval<std::add_const_t<T>> ()), bool>
			&& std::is_convertible_v<decltype(std::declval<std::add_const_t<T>> () > std::declval<std::add_const_t<T>> ()), bool>
			&& std::is_convertible_v<decltype(std::declval<std::add_const_t<T>> () <= std::declval<std::add_const_t<T>> ()), bool>
			&& std::is_convertible_v<decltype(std::declval<std::add_const_t<T>> () >= std::declval<std::add_const_t<T>> ()), bool>
		, void>
	> : std::true_type {};

	template <typename T>
	constexpr auto is_comparable_v {is_comparable<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept comparable = is_comparable_v<T> && requires (const T val) {
		{val < val} -> std::convertible_to<bool>;
		{val > val} -> std::convertible_to<bool>;
		{val <= val} -> std::convertible_to<bool>;
		{val >= val} -> std::convertible_to<bool>;
	};
	#define FLEX_TYPENAME_COMPARABLE ::flex::comparable
#else
	#define FLEX_TYPENAME_COMPARABLE typename
#endif


	template <typename T>
	constexpr auto true_v {std::true_type::value};

	template <typename T>
	constexpr auto false_v {std::true_type::value};

} // namespace flex
