#pragma once

#include <ranges>


namespace flex {
	template <typename T, typename Cont>
	class ContiguousIterator {
		public:
			using ;
	};


	static_assert(std::contiguous_iterator<ContiguousIterator<int, int>>);

} // namespace flex
