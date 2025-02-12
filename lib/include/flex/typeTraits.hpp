#pragma once

#include <cstdint>
#include <concepts>
#include <memory>


namespace flex {
	struct Empty {};

	template <typename T>
	concept IsAllocator = requires(T alloc, std::size_t n) {
		{*alloc.allocate(n)} -> std::same_as<typename T::value_type&>;
		{alloc.deallocate(alloc.allocate(n), n)};
	}
		&& std::copy_constructible<T>
		&& std::equality_comparable<T>;

	template <typename T>
	concept IsStatelessAllocator = std::is_empty_v<T> && IsAllocator<T>;

	template <typename T>
	concept IsStatefullAllocator = !IsStatelessAllocator<T> && IsAllocator<T>;


	template <typename T>
	struct AllocatorWrapper {
		using Type = std::remove_cvref_t<T>;
	};

	template <IsStatelessAllocator T>
	struct AllocatorWrapper<T> {
		using Type = Empty;
	};

	template <typename T>
	using AllocatorWrapper_t = typename AllocatorWrapper<T>::Type;

	static_assert(IsAllocator<std::allocator<int>>);
	static_assert(IsStatelessAllocator<std::allocator<int>>);
	static_assert(!IsStatefullAllocator<std::allocator<int>>);

} // namespace
