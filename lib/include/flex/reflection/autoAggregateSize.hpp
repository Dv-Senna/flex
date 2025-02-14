#pragma once

#include <cstddef>

#include "flex/typeTraits.hpp"


namespace flex::reflection {
	template <typename S, typename ...Placeholders>
	consteval auto getAggregateSize() noexcept -> std::size_t {
		if constexpr (flex::is_aggregate_constructible_v<S, Placeholders..., flex::AnyTypePlaceholder>)
			return getAggregateSize<S, Placeholders..., flex::AnyTypePlaceholder> ();
		else
			return sizeof...(Placeholders);
	};

} // namespace flex::reflection
