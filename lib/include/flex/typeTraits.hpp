#pragma once

#include <cstdint>
#include <concepts>
#include <memory>


namespace flex {
	template <typename T>
	concept IsAllocator = requires(T alloc, std::size_t n) {
		{*alloc.allocate(n)} -> std::same_as<typename T::value_type&>;
		{alloc.deallocate(alloc.allocate(n), n)};
	}
		&& std::copy_constructible<T>
		&& std::equality_comparable<T>;

	template <typename T>
	concept IsAllocatorOrVoid = std::is_void_v<T> || IsAllocator<T>;

	template <typename T>
	concept IsStatelessAllocator = std::is_empty_v<T> && IsAllocator<T>;

	template <typename T>
	concept IsStatefullAllocator = !IsStatelessAllocator<T> && IsAllocator<T>;


	static_assert(IsAllocator<std::allocator<int>>);
	static_assert(IsAllocatorOrVoid<std::allocator<int>>);
	static_assert(IsStatelessAllocator<std::allocator<int>>);
	static_assert(!IsStatefullAllocator<std::allocator<int>>);

	static_assert(!IsAllocator<void>);
	static_assert(IsAllocatorOrVoid<void>);
	static_assert(!IsStatelessAllocator<void>);
	static_assert(!IsStatefullAllocator<void>);

} // namespace
