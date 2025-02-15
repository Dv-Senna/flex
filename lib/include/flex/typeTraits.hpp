#pragma once

#include <tuple>
#include <type_traits>


namespace flex{
	struct AnyTypePlaceholder {
		template <typename T>
		operator T() const noexcept;
	};

	struct Empty {};


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

} // namespace flex
