#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>


namespace flex::memory {
	template<class Alloc>
	concept std_allocator = requires(Alloc alloc, std::size_t n)
	{
		{*alloc.allocate(n)} -> std::same_as<typename Alloc::value_type&>;
		{alloc.deallocate(alloc.allocate(n), n)};  
	} && std::copy_constructible<Alloc>
		&& std::equality_comparable<Alloc>;


	template <typename Alloc>
	concept allocator = requires(Alloc alloc, std::size_t n) {
		typename Alloc::ValueType;
	};


	template <allocator T>
	struct allocator_traits {
		using type = T;
		using ValueType = void;
	};

} // namespace flex::memory
