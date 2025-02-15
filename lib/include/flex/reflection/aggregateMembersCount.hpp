#pragma once

#include <cstddef>

#include "flex/typeTraits.hpp"


namespace flex::reflection {
	template <typename S, typename ...Placeholders>
	requires std::is_aggregate_v<S>
	consteval auto getAggregateMembersCount() noexcept -> std::size_t {
		if constexpr (flex::is_aggregate_constructible_v<S, Placeholders..., flex::AnyTypePlaceholder>)
			return getAggregateMembersCount<S, Placeholders..., flex::AnyTypePlaceholder> ();
		else
			return sizeof...(Placeholders);
	};

} // namespace flex::reflection
