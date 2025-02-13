#pragma once

#include "flex/iterator.hpp"

#include <print>


namespace flex {
	template <typename It>
	constexpr auto distance(const It &lhs, const It &rhs) noexcept -> typename iterator_traits<It>::ArithmeticType {
		if (lhs == rhs)
			return typename iterator_traits<It>::ArithmeticType{};

		if constexpr (flex::is_random_access_iterator_v<It>)
			return rhs - lhs;
		else if constexpr (flex::is_forward_iterator_v<It>) {
			typename iterator_traits<It>::ArithmeticType result {};
			for (auto it {lhs}; it != rhs; ++it, ++result);
			return result;
		}
		else
			return typename iterator_traits<It>::ArithmeticType{};
	};

} // namespace flex
