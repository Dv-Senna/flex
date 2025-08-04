#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>

#include "flex/containers/contiguousIterator.hpp"


namespace flex::containers {
	template <typename T, std::size_t capacity>
	class InplaceVector {
		public:
			using value_type = T;
			using reference = std::add_lvalue_reference_t<T>;
			using const_reference = std::add_lvalue_reference_t<std::add_const_t<T>>;
			using pointer = std::add_pointer_t<T>;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using iterator = flex::containers::ContiguousIterator<value_type, InplaceVector>;
			using const_iterator = flex::containers::ContiguousIterator<std::add_const_t<value_type>, InplaceVector>;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			constexpr InplaceVector() noexcept : m_size {0} {}
			constexpr ~InplaceVector() {
				for (auto& value : *this)
					value.~T();
				m_size = 0;
			}

			constexpr InplaceVector(std::initializer_list<T> init)
				noexcept (std::is_nothrow_copy_constructible_v<T>)
				requires (std::is_copy_constructible_v<T>)
			:
				m_size {0}
			{
				assert(init.size() <= capacity);
				for (const auto& value : init)
					this->pushBack(value);
			}
			template <std::input_iterator It>
			constexpr InplaceVector(It first, It last)
				noexcept (std::is_nothrow_convertible_v<std::iter_reference_t<It>, T>)
				requires std::convertible_to<std::iter_reference_t<It>, T>
			:
				m_size {0}
			{
				if constexpr (std::forward_iterator<decltype(first)>)
					assert(std::ranges::distance(first, last) <= capacity);
				for (; first != last; ++first)
					this->pushBack(*first);
			}
		#if defined(__cpp_lib_containers_ranges) && __cpp_lib_containers_ranges >= 202202L
			constexpr InplaceVector(std::from_range_t, std::ranges::input_range auto&& range)
				noexcept (std::is_nothrow_convertible_v<std::ranges::range_reference_t<decltype(range)>, T>)
				requires std::convertible_to<std::ranges::range_reference_t<decltype(range)>, T>
			:
				m_size {0}
			{
				if constexpr (std::ranges::forward_range<decltype(range)> || std::ranges::sized_range<decltype(range)>)
					assert(std::ranges::size(range) <= capacity);
				for (auto&& value : std::forward<decltype(range)> (range))
					this->pushBack(value);
			}
		#endif

			constexpr InplaceVector(const InplaceVector& other)
				noexcept (std::is_nothrow_copy_constructible_v<T>)
				requires std::is_copy_constructible_v<T>
			:
				m_size {0}
			{
				for (const auto& value : other)
					this->pushBack(value);
			}
			constexpr auto operator=(const InplaceVector& other)
				noexcept (
					std::is_nothrow_copy_constructible_v<T>
					&& (!std::is_copy_assignable_v<T> || std::is_nothrow_copy_assignable_v<T>)
				)
				-> InplaceVector&
				requires std::is_copy_constructible_v<T>
			{
				this->shrinkToSizeIfBigger(other.m_size);
				for (const auto i : std::views::iota(size_type{0}, m_size)) {
					if constexpr (std::is_copy_assignable_v<T>)
						m_data[i] = other.m_data[i];
					else {
						m_data[i].~T();
						new (&m_data[i]) T(other.m_data[i]);
					}
				}
				for (const auto& value : std::views::counted(other.begin() + m_size, other.m_size))
					this->pushBack(value);
				return *this;
			}

			constexpr InplaceVector(InplaceVector&& other)
				noexcept (std::is_nothrow_move_constructible_v<T>)
				requires std::is_move_constructible_v<T>
			:
				m_size {0}
			{
				for (const auto& value : other)
					this->pushBack(value);
				other.shrinkToSizeIfBigger(0);
			}
			constexpr auto operator=(InplaceVector&& other)
				noexcept (
					std::is_nothrow_move_constructible_v<T>
					&& (!std::is_move_assignable_v<T> || std::is_nothrow_move_assignable_v<T>)
				)
				-> InplaceVector&
				requires std::is_move_constructible_v<T>
			{
				this->shrinkToSizeIfBigger(other.m_size);
				for (const auto i : std::views::iota(size_type{0}, m_size)) {
					if constexpr (std::is_move_assignable_v<T>)
						m_data[i] = std::move(other.m_data[i]);
					else {
						m_data[i].~T();
						new (&m_data[i]) T(std::move(other.m_data[i]));
					}
				}
				for (auto& value : std::views::counted(other.begin() + m_size, other.m_size))
					this->pushBack(std::move(value));
				other.shrinkToSizeIfBigger(0);
				return *this;
			}

			constexpr auto begin() noexcept {return iterator{m_data};}
			constexpr auto end() noexcept {return iterator{m_data + m_size};}
			constexpr auto begin() const noexcept -> const_iterator {return this->cbegin();}
			constexpr auto end() const noexcept -> const_iterator {return this->cend();}
			constexpr auto cbegin() const noexcept {return const_iterator{m_data};}
			constexpr auto cend() const noexcept {return const_iterator{m_data + m_size};}

			constexpr auto rbegin() noexcept {return reverse_iterator{iterator{m_data + m_size}};}
			constexpr auto rend() noexcept {return reverse_iterator{iterator{m_data}};}
			constexpr auto rbegin() const noexcept -> const_reverse_iterator {return this->crbegin();}
			constexpr auto rend() const noexcept -> const_reverse_iterator {return this->crend();}
			constexpr auto crbegin() const noexcept {
				return const_reverse_iterator{const_iterator{m_data + m_size}};
			}
			constexpr auto crend() const noexcept {return const_reverse_iterator{const_iterator{m_data}};}

			constexpr auto size() const noexcept -> size_type {return m_size;}
			constexpr auto empty() const noexcept -> bool {return m_size == 0;}


			constexpr auto emplaceBack(auto&&... args)
				noexcept (std::is_nothrow_constructible_v<T, decltype(std::forward<decltype(args)> (args))...>)
				-> reference
				requires std::constructible_from<T, decltype(std::forward<decltype(args)> (args))...>
			{
				assert(m_size + 1 <= capacity);
				reference data {m_data[m_size++]};
				new (&data) T(std::forward<decltype(args)> (args)...);
				return data;
			}

			constexpr auto pushBack(const_reference value)
				noexcept (std::is_nothrow_copy_constructible_v<T>)
				-> reference
				requires std::is_copy_constructible_v<T>
			{
				return this->emplaceBack(value);
			}

			constexpr auto pushBack(std::add_rvalue_reference_t<T> value)
				noexcept (std::is_nothrow_move_constructible_v<T>)
				-> reference
				requires std::move_constructible<T>
			{
				return this->emplaceBack(std::move(value));
			}


			constexpr auto tryEmplaceBack(auto&&... args)
				noexcept (std::is_nothrow_constructible_v<T, decltype(std::forward<decltype(args)> (args))...>)
				-> std::optional<std::reference_wrapper<value_type>>
				requires std::constructible_from<T, decltype(std::forward<decltype(args)> (args))...>
			{
				if (m_size + 1 > capacity)
					return std::nullopt;
				reference data {m_data[m_size++]};
				new (&data) T(std::forward<decltype(args)> (args)...);
				return data;
			}

			constexpr auto tryPushBack(const_reference value)
				noexcept (std::is_nothrow_copy_constructible_v<T>)
				-> std::optional<std::reference_wrapper<value_type>>
				requires std::is_copy_constructible_v<T>
			{
				return this->tryEmplaceBack(value);
			}

			constexpr auto tryPushBack(std::add_rvalue_reference_t<T> value)
				noexcept (std::is_nothrow_move_constructible_v<T>)
				-> std::optional<std::reference_wrapper<value_type>>
				requires std::move_constructible<T>
			{
				return this->tryEmplaceBack(std::move(value));
			}


			constexpr auto operator[](std::unsigned_integral auto pos) noexcept -> reference {
				assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
				return m_data[pos];
			}
			constexpr auto operator[](std::unsigned_integral auto pos) const noexcept -> const_reference {
				assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
				return m_data[pos];
			}

			constexpr auto operator[](std::signed_integral auto pos) noexcept -> reference {
				if (pos >= 0) {
					assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
					return m_data[pos];
				}
				else {
					assert(static_cast<std::uintmax_t> (-pos) <= static_cast<std::uintmax_t> (m_size));
					return m_data[m_size + pos];
				}
			}
			constexpr auto operator[](std::signed_integral auto pos) const noexcept -> const_reference {
				if (pos >= 0) {
					assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
					return m_data[pos];
				}
				else {
					assert(static_cast<std::uintmax_t> (-pos) <= static_cast<std::uintmax_t> (m_size));
					return m_data[m_size + pos];
				}
			}


		private:
			constexpr auto shrinkToSizeIfBigger(size_type size) noexcept -> void {
				if (m_size <= size)
					return;
				for (const auto i: std::views::iota(size, m_size))
					m_data[i].~T();
				m_size = size;
			}

			union {
				T m_data[capacity];
			};
			size_type m_size;
	};

	namespace internals {
		template <typename ...Args>
		using make_inplace_vector_value_type_t = typename std::remove_cvref<
			typename std::tuple_element<0, std::tuple<Args...>>::type
		>::type;

		template <typename ...Args>
		static constexpr auto make_inplace_vector_noexcept_v = (std::is_nothrow_constructible<
			make_inplace_vector_value_type_t<Args...>,
			decltype(std::forward<Args> (std::declval<Args> ()))
		>::value && ...);

		template <std::size_t capacity, typename ...Args>
		static constexpr auto is_make_inplace_vector_v = sizeof...(Args) > 0
			&& sizeof...(Args) <= capacity
			&& (std::is_constructible<
				make_inplace_vector_value_type_t<Args...>,
				decltype(std::forward<Args> (std::declval<Args> ()))
			>::value && ...)
			&& (
				std::is_move_constructible<
					InplaceVector<internals::make_inplace_vector_value_type_t<Args...>, capacity>
				>::value
				|| std::is_copy_constructible<
					InplaceVector<internals::make_inplace_vector_value_type_t<Args...>, capacity>
				>::value
			);
	}

	template <std::size_t capacity>
	constexpr auto makeInplaceVector(auto&&... args)
		noexcept (internals::make_inplace_vector_noexcept_v<decltype(args)...>)
		-> InplaceVector<internals::make_inplace_vector_value_type_t<decltype(args)...>, capacity>
		requires (internals::is_make_inplace_vector_v<capacity, decltype(args)...>)
	{
		InplaceVector<internals::make_inplace_vector_value_type_t<decltype(args)...>, capacity> result {};
		std::tuple argsAsTuple {std::forward<decltype(args)> (args)...};
		auto loop {[&] <std::size_t I = 0> (auto loop) {
			using Current = std::tuple_element_t<I, std::tuple<decltype(args)...>>;
			result.pushBack(std::forward<Current> (std::get<I> (argsAsTuple)));
			if constexpr (I + 1 < sizeof...(args))
				loop.template operator() <I + 1> (loop);
		}};
		loop(loop);
		return result;
	}


	template <std::ranges::contiguous_range T>
	constexpr bool test() {return true;}
	static_assert(test<InplaceVector<int, 16>> ());
}
