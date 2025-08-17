#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>

#include "flex/containers/contiguousIterator.hpp"


namespace flex::containers {
	template <typename T, std::size_t capacity, bool forceTrivialBehaviour = false>
	requires (!forceTrivialBehaviour || std::is_default_constructible_v<T>)
	class InplaceVector {
		public:
			using value_type = T;
			using reference = std::add_lvalue_reference_t<T>;
			using const_reference = std::add_lvalue_reference_t<std::add_const_t<T>>;
			using pointer = std::add_pointer_t<T>;
			using const_pointer = std::add_pointer_t<std::add_const_t<T>>;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using iterator = flex::containers::ContiguousIterator<value_type, InplaceVector>;
			using const_iterator = flex::containers::ContiguousIterator<std::add_const_t<value_type>, InplaceVector>;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			constexpr InplaceVector() noexcept : m_size {0} {}
			constexpr ~InplaceVector() {
				this->shrinkToSizeIfBigger(0);
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
				this->insert(this->cend(), first, last);
			}
		#if defined(__cpp_lib_containers_ranges) && __cpp_lib_containers_ranges >= 202202L
			constexpr InplaceVector(std::from_range_t, std::ranges::input_range auto&& range)
				noexcept (std::is_nothrow_convertible_v<std::ranges::range_reference_t<decltype(range)>, T>)
				requires std::convertible_to<std::ranges::range_reference_t<decltype(range)>, T>
			:
				m_size {0}
			{
				this->appendRange(std::forward<decltype(range)> (range));
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
						this->at(i) = other.at(i);
					else {
						this->at(i).~T();
						this->constructAt(i, other.at(i));
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
				for (auto& value : std::move(other))
					this->pushBack(std::move(value));
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
						this->at(i) = std::move(other.at(i));
					else {
						this->at(i).~T();
						this->constructAt(i, std::move(other.at(i)));
					}
				}
				for (auto& value : std::views::counted(other.begin() + m_size, other.m_size))
					this->pushBack(std::move(value));
				other.shrinkToSizeIfBigger(0);
				return *this;
			}

			[[nodiscard]]
			constexpr auto begin() noexcept {return iterator{&this->at(0)};}
			[[nodiscard]]
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			constexpr auto end() noexcept {return iterator{&this->at(0) + m_size};}
			[[nodiscard]]
			constexpr auto begin() const noexcept -> const_iterator {return this->cbegin();}
			[[nodiscard]]
			constexpr auto end() const noexcept -> const_iterator {return this->cend();}
			[[nodiscard]]
			constexpr auto cbegin() const noexcept {return const_iterator{&this->at(0)};}
			[[nodiscard]]
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			constexpr auto cend() const noexcept {return const_iterator{&this->at(0) + m_size};}

			[[nodiscard]]
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			constexpr auto rbegin() noexcept {return reverse_iterator{iterator{&this->at(0) + m_size}};}
			[[nodiscard]]
			constexpr auto rend() noexcept {return reverse_iterator{iterator{&this->at(0)}};}
			[[nodiscard]]
			constexpr auto rbegin() const noexcept -> const_reverse_iterator {return this->crbegin();}
			[[nodiscard]]
			constexpr auto rend() const noexcept -> const_reverse_iterator {return this->crend();}
			[[nodiscard]]
			constexpr auto crbegin() const noexcept {
				return const_reverse_iterator{const_iterator{&this->at(0) + m_size}};
			}
			[[nodiscard]]
			constexpr auto crend() const noexcept {return const_reverse_iterator{const_iterator{&this->at(0)}};}

			[[nodiscard]]
			constexpr auto size() const noexcept -> size_type {return m_size;}
			[[nodiscard]]
			constexpr auto empty() const noexcept -> bool {return m_size == 0;}


			constexpr auto emplaceBack(auto&&... args)
				noexcept (std::is_nothrow_constructible_v<T, decltype(std::forward<decltype(args)> (args))...>)
				-> reference
				requires std::constructible_from<T, decltype(std::forward<decltype(args)> (args))...>
			{
				assert(m_size + 1 <= capacity);
				return this->constructAt(m_size++, std::forward<decltype(args)> (args)...);
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
				return this->constructAt(m_size++, std::forward<decltype(args)> (args)...);
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


			template <std::input_iterator It>
			constexpr auto insert(const_iterator pos, It first, It last)
				noexcept (std::is_nothrow_convertible_v<std::iter_reference_t<It>, T>)
				-> iterator
				requires std::convertible_to<std::iter_reference_t<It>, T>
			{
				assert(this->isIteratorValid(pos));
				const auto offset {static_cast<size_type> (pos - this->cbegin())};
				if constexpr (std::forward_iterator<It>)
					assert(std::ranges::distance(first, last) + offset <= capacity);

				iterator result {this->begin() + (pos - this->cbegin())};
				for (; first != last; ++first)
					this->pushBack(static_cast<T> (*first));
				return result;
			}

			constexpr auto insert(const_iterator pos, std::ranges::input_range auto&& range)
				noexcept (std::is_nothrow_convertible_v<std::ranges::range_reference_t<decltype(range)>, T>)
				-> iterator
				requires std::convertible_to<std::ranges::range_reference_t<decltype(range)>, T>
			{
				return this->insert(pos,
					std::ranges::begin(std::forward<decltype(range)> (range)),
					std::ranges::end(std::forward<decltype(range)> (range))
				);
			}

			constexpr auto appendRange(std::ranges::input_range auto&& range)
				noexcept (std::is_nothrow_convertible_v<std::ranges::range_reference_t<decltype(range)>, T>)
				-> iterator
				requires std::convertible_to<std::ranges::range_reference_t<decltype(range)>, T>
			{
				return this->insert(this->cend(), std::forward<decltype(range)> (range));
			}


			constexpr auto erase(const_iterator start, const_iterator end)
				noexcept (std::is_nothrow_move_assignable_v<T> || std::is_nothrow_copy_assignable_v<T>)
				-> iterator
				requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>)
			{
				assert(this->isIteratorValid(start));
				assert(this->isIteratorValid(end));
				const auto tailSize {static_cast<size_type> (this->cend() - end)};
				const auto startOffset {static_cast<size_type> (start - this->cbegin())};
				const auto tailOffset {static_cast<size_type> (end - this->cbegin())};
				const auto removeCount {static_cast<size_type> (end - start)};

				const iterator result {this->begin() + static_cast<difference_type> (startOffset)};
				if (removeCount == 0)
					return result;
				if (tailSize != 0) {
					for (const auto i : std::views::iota(size_type{0}, tailSize)) {
						if constexpr (std::is_move_assignable_v<T>)
							this->at(startOffset + i) = std::move(this->at(tailOffset + i));
						else
							this->at(startOffset + i) = this->at(tailOffset + i);
						this->at(tailOffset + i).~T();
						if constexpr (useTrivialImplementation)
							this->constructAt(tailOffset + i);
					}
				}
				if (tailSize >= removeCount) {
					m_size -= removeCount;
					return result;
				}

				for (const auto i : std::views::iota(startOffset + tailSize, tailOffset)) {
					this->at(i).~T();
					if constexpr (useTrivialImplementation)
						this->constructAt(i);
				}
				m_size -= removeCount;
				return result;
			}


			constexpr auto erase(std::ranges::input_range auto&& range)
				noexcept (std::is_nothrow_move_assignable_v<T> || std::is_nothrow_copy_assignable_v<T>)
				-> iterator
				requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>)
			{
				return this->erase(
					std::ranges::begin(std::forward<decltype(range)> (range)),
					std::ranges::end(std::forward<decltype(range)> (range))
				);
			}


			constexpr auto operator[](std::unsigned_integral auto pos) noexcept -> reference {
				assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
				return this->at(pos);
			}
			constexpr auto operator[](std::unsigned_integral auto pos) const noexcept -> const_reference {
				assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
				return this->at(pos);
			}

			constexpr auto operator[](std::signed_integral auto pos) noexcept -> reference {
				if (pos >= 0) {
					assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
					return this->at(pos);
				}
				else {
					assert(static_cast<std::uintmax_t> (-pos) <= static_cast<std::uintmax_t> (m_size));
					return this->at(m_size + pos);
				}
			}
			constexpr auto operator[](std::signed_integral auto pos) const noexcept -> const_reference {
				if (pos >= 0) {
					assert(static_cast<std::uintmax_t> (pos) < static_cast<std::uintmax_t> (m_size));
					return this->at(pos);
				}
				else {
					assert(static_cast<std::uintmax_t> (-pos) <= static_cast<std::uintmax_t> (m_size));
					return this->at(m_size + pos);
				}
			}


		private:
			static constexpr auto useTrivialImplementation = std::is_trivially_constructible_v<T>
				|| forceTrivialBehaviour;

			[[nodiscard]]
			constexpr auto atAsPointer(size_type index) noexcept -> pointer {
				if constexpr (useTrivialImplementation)
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
					return static_cast<pointer> (m_storage) + index;
				else
					return &m_storage[0].data + index;
			}

			[[nodiscard]]
			constexpr auto atAsPointer(size_type index) const noexcept -> const_pointer {
				if constexpr (useTrivialImplementation)
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
					return static_cast<const_pointer> (m_storage) + index;
				else
					return &m_storage[0].data + index;
			}

			[[nodiscard]]
			constexpr auto at(size_type index) noexcept -> reference {
				return *this->atAsPointer(index);
			}

			[[nodiscard]]
			constexpr auto at(size_type index) const noexcept -> const_reference {
				return *this->atAsPointer(index);
			}

			constexpr auto constructAt(size_type index, auto&&... args)
				noexcept (std::is_nothrow_constructible_v<T, decltype(std::forward<decltype(args)> (args))...>)
				-> reference
				requires std::constructible_from<T, decltype(std::forward<decltype(args)> (args))...>
			{
				assert(index < m_size);
				std::construct_at(this->atAsPointer(index), std::forward<decltype(args)> (args)...);
				return this->at(index);
			}

			constexpr auto shrinkToSizeIfBigger(size_type size) noexcept -> void {
				if (m_size <= size)
					return;
				for (const auto i: std::views::iota(size, m_size)) {
					this->at(i).~T();
					if constexpr (useTrivialImplementation)
						this->constructAt(i);
				}
				m_size = size;
			}

			[[nodiscard]]
			constexpr auto isIteratorValid(const_iterator it) const noexcept -> bool {
				return it - this->cbegin() >= 0
					&& static_cast<std::uintmax_t> (it - this->cbegin()) <= static_cast<std::uintmax_t> (m_size);
			}

		// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
			union Storage {
				T data;
				constexpr Storage() noexcept {}
				constexpr ~Storage() {}
			};
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
			std::conditional_t<useTrivialImplementation, T, Storage> m_storage[capacity];
			size_type m_size;
	};

	namespace internals {
		template <typename ...Args>
		using make_inplace_vector_value_type_t = std::remove_cvref_t<
			std::tuple_element_t<0, std::tuple<Args...>>
		>;

		template <typename ...Args>
		static constexpr auto make_inplace_vector_noexcept_v = (std::is_nothrow_constructible_v<
			make_inplace_vector_value_type_t<Args...>,
			decltype(std::forward<Args> (std::declval<Args> ()))
		> && ...);

		template <std::size_t capacity, typename ...Args>
		static constexpr auto is_make_inplace_vector_v = sizeof...(Args) > 0
			&& sizeof...(Args) <= capacity
			&& (std::is_constructible_v<
				make_inplace_vector_value_type_t<Args...>,
				decltype(std::forward<Args> (std::declval<Args> ()))
			> && ...)
			&& (
				std::is_move_constructible_v<
					InplaceVector<internals::make_inplace_vector_value_type_t<Args...>, capacity>
				>
				|| std::is_copy_constructible_v<
					InplaceVector<internals::make_inplace_vector_value_type_t<Args...>, capacity>
				>
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


// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
	static_assert(std::ranges::contiguous_range<InplaceVector<int, 16>>);
}
