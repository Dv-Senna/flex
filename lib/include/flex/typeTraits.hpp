#pragma once

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
	constexpr auto is_aggregate_constructible_v {is_aggregate_constructible<T, void, Args...>::value};

} // namespace flex
