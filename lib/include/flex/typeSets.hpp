#pragma once

#include <type_traits>

#include "flex/typeTraits.hpp"


namespace flex {
	template <flex::tuple ...Tuples>
	struct merge_tuple : flex::type_constant<decltype(std::tuple_cat(std::declval<Tuples> ()...))> {};

	template <flex::tuple ...Tuples>
	using merge_tuple_t = typename merge_tuple<Tuples...>::type;

	static_assert(std::is_same_v<std::tuple<int, float>, merge_tuple_t<std::tuple<int>, std::tuple<float>>>);


	template <flex::tuple T>
	struct pop_first_tuple_element : flex::type_constant<T> {};

	template <typename First, typename ...Args>
	struct pop_first_tuple_element<std::tuple<First, Args...>> : flex::type_constant<std::tuple<Args...>> {};

	template <flex::tuple T>
	using pop_first_tuple_element_t = typename pop_first_tuple_element<T>::type;

	static_assert(std::is_same_v<std::tuple<float>, pop_first_tuple_element_t<std::tuple<int, float>>>);


	template <typename T, flex::tuple Tuple, std::size_t INDEX = 0, std::size_t SIZE = std::tuple_size_v<Tuple>>
	struct has_tuple_element_type : std::bool_constant<
		std::is_same_v<T, std::tuple_element_t<INDEX, Tuple>>
		|| has_tuple_element_type<T, Tuple, INDEX+1, SIZE>::value
	> {};

	template <typename T, flex::tuple Tuple, std::size_t SIZE>
	struct has_tuple_element_type<T, Tuple, SIZE, SIZE> : std::false_type {};

	template <typename T, flex::tuple Tuple>
	constexpr auto has_tuple_element_type_v = has_tuple_element_type<T, Tuple>::value;

	static_assert(has_tuple_element_type_v<int, std::tuple<int, float>>);
	static_assert(has_tuple_element_type_v<float, std::tuple<int, float>>);
	static_assert(!has_tuple_element_type_v<char, std::tuple<int, float>>);


	template <flex::tuple T, typename = void>
	struct remove_tuple_duplicate : flex::type_constant<flex::merge_tuple_t<
		std::tuple<std::tuple_element_t<0, T>>, typename remove_tuple_duplicate<pop_first_tuple_element_t<T>>::type
	>> {};

	template <flex::tuple T>
	struct remove_tuple_duplicate<T, std::enable_if_t<
		has_tuple_element_type_v<std::tuple_element_t<0, T>, pop_first_tuple_element_t<T>>
	, void>> : flex::type_constant<
		typename remove_tuple_duplicate<pop_first_tuple_element_t<T>>::type
	> {};

	template <>
	struct remove_tuple_duplicate<std::tuple<>> : flex::type_constant<std::tuple<>> {};

	template <flex::tuple T>
	using remove_tuple_duplicate_t = typename remove_tuple_duplicate<T>::type;

	static_assert(std::is_same_v<std::tuple<int, float>, remove_tuple_duplicate_t<std::tuple<float, int, float, float, int, float>>>);


	template <flex::tuple T1, flex::tuple T2, std::size_t INDEX = 0, std::size_t SIZE = std::tuple_size_v<T1>>
	struct is_same_sets : std::bool_constant<
		std::tuple_size_v<T2> == SIZE
		&& has_tuple_element_type_v<std::tuple_element_t<INDEX, T1>, T2>
		&& is_same_sets<T1, T2, INDEX+1, SIZE>::value
	> {};

	template <flex::tuple T1, flex::tuple T2, std::size_t SIZE>
	struct is_same_sets<T1, T2, SIZE, SIZE> : std::true_type {};

	template <flex::tuple T1, flex::tuple T2>
	struct is_same_sets<T1, T2, 0, 0> : std::bool_constant<
		std::tuple_size_v<T2> == 0
	> {};

	template <flex::tuple T1, flex::tuple T2>
	constexpr auto is_same_sets_v = is_same_sets<T1, T2>::value;

	static_assert(is_same_sets_v<std::tuple<int, float>, std::tuple<int, float>>);
	static_assert(is_same_sets_v<std::tuple<int, float>, std::tuple<float, int>>);

} // namespace flex
