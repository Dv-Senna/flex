#pragma once

#include "flex/config.hpp"

#include <cstddef>
#include <concepts>
#include <memory>

#include "flex/typeTraits.hpp"


namespace flex {
	template <typename T, typename = void>
	struct is_allocator : std::false_type {};

	template <typename T>
	struct is_allocator<T,
		std::enable_if_t<
			std::is_copy_constructible_v<T>
			&& flex::is_equality_comparable_v<T>
			&& std::is_same_v<typename T::value_type&, decltype(*std::declval<T> ().allocate(std::size_t{}))>
			&& flex::true_v<decltype(std::declval<T> ().deallocate(std::declval<T> ().allocate(std::size_t{}), std::size_t{}))>,
		void>
	> : std::true_type {};

	template <typename T>
	constexpr auto is_allocator_v {is_allocator<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept allocator = flex::is_allocator_v<T>
		&& flex::equality_comparable<T>
		&& requires(T alloc, std::size_t n) {
		{*alloc.allocate(n)} -> std::same_as<typename T::value_type&>;
		{alloc.deallocate(alloc.allocate(n), n)};
	};
#endif // concepts


	template <typename T>
	struct is_stateless_allocator : std::bool_constant<std::is_empty_v<T> && flex::is_allocator_v<T>> {};

	template <typename T>
	constexpr auto is_stateless_allocator_v {is_stateless_allocator<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept stateless_allocator = flex::is_stateless_allocator_v<T>
		&& flex::allocator<T>;
#endif // concepts


	template <typename T>
	struct is_statefull_allocator : std::bool_constant<!flex::is_stateless_allocator_v<T> && flex::is_allocator_v<T>> {};

	template <typename T>
	constexpr auto is_statefull_allocator_v {is_statefull_allocator<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept statefull_allocator = flex::is_statefull_allocator_v<T>
		&& flex::allocator<T>;
#endif // concepts


	template <typename T, typename = void>
	struct remove_allocator {
		using type = std::remove_cvref_t<T>;
	};

	template <typename T>
	struct remove_allocator<T,
		std::enable_if_t<is_stateless_allocator_v<T>, void>
	> {
		using type = flex::Empty;
	};

	template <typename T>
	using remove_allocator_t = typename remove_allocator<T>::type;



	static_assert(is_allocator_v<std::allocator<int>>);
	static_assert(is_stateless_allocator_v<std::allocator<int>>);
	static_assert(!is_statefull_allocator_v<std::allocator<int>>);

} // namespace flex
