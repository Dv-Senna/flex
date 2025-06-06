#pragma once

#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>


namespace flex{
	struct AnyTypePlaceholder {
		template <typename T>
		operator T() const noexcept;
	};

	struct Empty {};


	template <typename T>
	constexpr bool true_v = true;

	template <typename T>
	constexpr bool false_v = false;


	template <typename T>
	struct type_constant {
		using type = T;
	};

	template <auto VALUE>
	struct value_constant {
		using value_type = decltype(VALUE);
		static constexpr auto value = VALUE;
	};


	template <typename T>
	concept reference = std::is_reference_v<T>;

	template <typename T>
	concept pointer = std::is_pointer_v<T>;

	template <typename T>
	concept value = !reference<T> && !pointer<T>;

	template <typename T>
	concept value_or_pointer = value<T> || pointer<T>;

	template <typename T>
	concept value_or_reference = value<T> || reference<T>;

	template <typename T>
	concept pointer_or_reference = pointer<T> || reference<T>;


	template <typename T>
	concept cv_reference = reference<T> || std::is_const_v<T> || std::is_volatile_v<T>;

	template <typename T>
	concept no_cv_reference = !cv_reference<T>;


	template <bool cond, typename T>
	struct enable_field_if : type_constant<T> {};

	template <typename T>
	struct enable_field_if<false, T> : type_constant<Empty> {};

	template <bool cond, typename T>
	using enable_field_if_t = typename enable_field_if<cond, T>::type;


	template <typename T, typename = void, typename ...Args>
	struct is_aggregate_constructible : std::false_type {};

	template <typename T, typename ...Args>
	struct is_aggregate_constructible<T,
		std::void_t<decltype(T{std::declval<Args> ()...})>
	, Args...> : std::true_type {};

	template <typename T, typename ...Args>
	constexpr auto is_aggregate_constructible_v = is_aggregate_constructible<T, void, Args...>::value;

	template <typename T, typename ...Args>
	concept aggregate_constructible = is_aggregate_constructible_v<T, Args...>;


	template <typename T>
	concept aggregate = std::is_aggregate_v<T>;


	template <typename T>
	struct remove_tuple_reference {
		using type = T;
	};

	template <typename ...Args>
	struct remove_tuple_reference<std::tuple<Args...>> {
		using type = std::tuple<std::remove_reference_t<Args>...>;
	};

	template <typename T>
	using remove_tuple_reference_t = typename remove_tuple_reference<T>::type;


	template <typename T>
	struct is_tuple : std::false_type {};

	template <typename ...Args>
	struct is_tuple<std::tuple<Args...>> : std::true_type {};

	template <typename T>
	constexpr auto is_tuple_v = is_tuple<T>::value;

	template <typename T>
	concept tuple = is_tuple_v<T>;


	template <typename T, typename = void>
	struct sfinae_tuple_size : std::integral_constant<std::size_t, std::numeric_limits<std::size_t>::max()> {};

	template <tuple T>
	struct sfinae_tuple_size<T> : std::integral_constant<std::size_t, std::tuple_size_v<T>> {};

	template <typename T>
	constexpr auto sfinae_tuple_size_v = sfinae_tuple_size<T>::value;


	template <typename T>
	struct is_pair : std::false_type {};

	template <typename First, typename Second>
	struct is_pair<std::pair<First, Second>> : std::true_type {};

	template <typename T>
	constexpr auto is_pair_v = is_pair<T>::value;

	template <typename T>
	concept pair = is_pair_v<T>;


	template <typename T>
	struct remove_member_function_pointer_const_noexcept {
		using type = T;
	};

	template <typename S, typename Ret, typename ...Args>
	struct remove_member_function_pointer_const_noexcept<Ret (S::*)(Args...) noexcept> {
		using type = Ret (S::*)(Args...);
	};

	template <typename S, typename Ret, typename ...Args>
	struct remove_member_function_pointer_const_noexcept<Ret (S::*)(Args...) const> {
		using type = Ret (S::*)(Args...);
	};

	template <typename S, typename Ret, typename ...Args>
	struct remove_member_function_pointer_const_noexcept<Ret (S::*)(Args...) const noexcept> {
		using type = Ret (S::*)(Args...);
	};

	template <typename T>
	using remove_member_function_pointer_const_noexcept_t = typename remove_member_function_pointer_const_noexcept<T>::type;


	template <typename T>
	struct member_pointer_extractor {
		using type = T;
	};

	template <typename S, typename T>
	struct member_pointer_extractor<T S::*> {
		using type = T;
	};

	template <typename S, typename Ret, typename ...Args>
	struct member_pointer_extractor<Ret (S::*)(Args...)> {
		using type = Ret (*)(Args...);
	};

	template <typename S, typename Ret, typename ...Args>
	struct member_pointer_extractor<Ret (S::*)(Args...) const> {
		using type = Ret (*)(Args...);
	};

	template <typename S, typename Ret, typename ...Args>
	struct member_pointer_extractor<Ret (S::*)(Args...) noexcept> {
		using type = Ret (*)(Args...) noexcept;
	};

	template <typename S, typename Ret, typename ...Args>
	struct member_pointer_extractor<Ret (S::*)(Args...) const noexcept> {
		using type = Ret (*)(Args...) noexcept;
	};

	template <typename T>
	using member_pointer_extractor_t = typename member_pointer_extractor<T>::type;


	template <typename T>
	struct remove_pointer_or_extent {
		using type = T;
	};

	template <typename T>
	struct remove_pointer_or_extent<T*> {
		using type = T;
	};

	template <typename T>
	struct remove_pointer_or_extent<T[]> {
		using type = T;
	};

	template <typename T, std::size_t N>
	struct remove_pointer_or_extent<T[N]> {
		using type = T;
	};

	template <typename T>
	using remove_pointer_or_extent_t = typename remove_pointer_or_extent<T>::type;


	template <bool COND, typename T>
	struct if_add_const {
		using type = T;
	};

	template <typename T>
	struct if_add_const<true, T> {
		using type = std::add_const_t<T>;
	};

	template <bool COND, typename T>
	using if_add_const_t = typename if_add_const<COND, T>::type;


	template <typename T>
	struct remove_function_parameter_cvref {
		using type = T;
	};

	template <typename Ret, typename ...Args>
	struct remove_function_parameter_cvref<Ret(Args...)> {
		using type = Ret(std::remove_cvref_t<Args>...);
	};

	template <typename Ret, typename ...Args>
	struct remove_function_parameter_cvref<Ret(Args...) noexcept> {
		using type = Ret(std::remove_cvref_t<Args>...) noexcept;
	};

	template <typename Ret, typename ...Args>
	struct remove_function_parameter_cvref<Ret (*)(Args...)> {
		using type = Ret(*)(std::remove_cvref_t<Args>...);
	};

	template <typename Ret, typename ...Args>
	struct remove_function_parameter_cvref<Ret (*)(Args...) noexcept> {
		using type = Ret(*)(std::remove_cvref_t<Args>...) noexcept;
	};

	template <typename T>
	using remove_function_parameter_cvref_t = typename remove_function_parameter_cvref<T>::type;


	template <typename T>
	concept arithmetic = std::integral<T> || std::floating_point<T>;


	template <typename T>
	constexpr auto is_string_v = std::is_same_v<T, std::string_view>
		|| std::is_same_v<T, std::string>
		|| (std::is_array_v<T> && std::is_same_v<std::remove_const_t<std::remove_extent_t<T>>, char>)
		|| (std::is_pointer_v<T> && std::is_same_v<std::remove_const_t<std::remove_pointer_t<T>>, char>);

	template <typename T>
	concept string = is_string_v<T>;


	template <typename T, typename = void>
	struct is_stringifyable : std::false_type {};

	template <arithmetic T>
	struct is_stringifyable<T> : std::true_type {};

	template <string T>
	struct is_stringifyable<T> : std::true_type {};

	template <>
	struct is_stringifyable<bool> : std::true_type {};

	template <typename T>
	constexpr auto is_stringifyable_v = is_stringifyable<T>::value;

	template <typename T>
	concept stringifyable = is_stringifyable_v<T>;



	template <typename T>
	struct is_optional : std::false_type {};

	template <typename T>
	struct is_optional<std::optional<T>> : std::true_type {};

	template <typename T>
	constexpr auto is_optional_v = is_optional<T>::value;

	template <typename T>
	concept optional = is_optional_v<T>;


	template <typename T>
	struct fully_unwrap_optional : flex::type_constant<T> {};

	template <optional T>
	struct fully_unwrap_optional<T> : flex::type_constant<typename fully_unwrap_optional<typename T::value_type>::type> {};

	template <typename T>
	using fully_unwrap_optional_t = typename fully_unwrap_optional<T>::type;


	template <bool COND, auto TRUE_VAL, auto FALSE_VAL>
	struct conditional_value : value_constant<FALSE_VAL> {};

	template <auto TRUE_VAL, auto FALSE_VAL>
	struct conditional_value<true, TRUE_VAL, FALSE_VAL> : value_constant<TRUE_VAL> {};

	template <bool COND, auto TRUE_VAL, auto FALSE_VAL>
	constexpr auto conditional_value_v = conditional_value<COND, TRUE_VAL, FALSE_VAL>::value;


	template <template <typename...> typename T, typename ...Args>
	struct partial_template_apply {
		template <typename ...Args2>
		using type = T<Args..., Args2...>;
	};

} // namespace flex
