#pragma once

#include <cstddef>
#include <type_traits>

#include "flex/typeTraits.hpp"


namespace flex::reflection {
	template <flex::aggregate T, typename = void, typename ...Placeholders>
	struct aggregate_members_count : std::integral_constant<std::size_t, sizeof...(Placeholders)> {};

	template <flex::aggregate T, typename ...Placeholders>
	struct aggregate_members_count<T,
		std::void_t<decltype(T{std::declval<Placeholders> ()..., std::declval<flex::AnyTypePlaceholder> ()})>
	, Placeholders...> : aggregate_members_count<T, void, Placeholders..., flex::AnyTypePlaceholder> {};

	template <flex::aggregate T>
	constexpr auto aggregate_members_count_v = aggregate_members_count<T>::value;

} // namespace flex::reflection
