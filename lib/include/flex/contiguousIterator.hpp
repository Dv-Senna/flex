#pragma once

#include <compare>
#include <iterator>
#include <type_traits>


namespace flex {
	template <typename T, typename Cont>
	class ContiguousIterator {
		using Container = std::remove_const_t<std::remove_reference_t<Cont>>;
		friend Container;

		public:
			using iterator_concept = std::contiguous_iterator_tag;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = std::remove_reference_t<T>;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using reference = value_type&;

			constexpr ContiguousIterator() noexcept = default;
			constexpr ~ContiguousIterator() = default;

			constexpr ContiguousIterator(const ContiguousIterator<T, Cont>&) noexcept = default;
			constexpr auto operator=(const ContiguousIterator<T, Cont>&) noexcept -> ContiguousIterator& = default;
			constexpr ContiguousIterator(ContiguousIterator<T, Cont>&&) noexcept = default;
			constexpr auto operator=(ContiguousIterator<T, Cont>&&) noexcept -> ContiguousIterator& = default;

			constexpr auto operator==(const ContiguousIterator<T, Cont> &it) const noexcept -> bool;
			constexpr auto operator<=>(const ContiguousIterator<T, Cont> &it) const noexcept -> std::partial_ordering;

			constexpr auto operator++() noexcept -> ContiguousIterator<T, Cont>&;
			constexpr auto operator--() noexcept -> ContiguousIterator<T, Cont>&;
			constexpr auto operator++(int) noexcept -> ContiguousIterator<T, Cont>;
			constexpr auto operator--(int) noexcept -> ContiguousIterator<T, Cont>;

			constexpr auto operator+=(difference_type value) noexcept -> ContiguousIterator<T, Cont>&;
			constexpr auto operator-=(difference_type value) noexcept -> ContiguousIterator<T, Cont>&;

			constexpr auto operator+(difference_type value) const noexcept -> ContiguousIterator<T, Cont>;
			friend constexpr auto operator+(difference_type value, ContiguousIterator<T, Cont> it) noexcept -> ContiguousIterator<T, Cont>;
			constexpr auto operator-(difference_type value) const noexcept -> ContiguousIterator<T, Cont>;

			constexpr auto operator-(const ContiguousIterator<T, Cont> &it) const noexcept -> difference_type;

			constexpr auto operator[](difference_type value) const noexcept -> reference;
			constexpr auto operator*() const noexcept -> reference;
			constexpr auto operator->() const noexcept -> pointer;


		protected:
			constexpr ContiguousIterator(pointer ptr, const Container *container) noexcept;

		private:
			pointer m_ptr;
			const Container *m_container;
	};


	template <typename T, typename Cont>
	class ReverseContiguousIterator {
		using Container = std::remove_const_t<std::remove_reference_t<Cont>>;
		friend Container;

		public:
			using iterator_concept = std::contiguous_iterator_tag;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = std::remove_reference_t<T>;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using reference = value_type&;

			constexpr ReverseContiguousIterator() noexcept = default;
			constexpr ~ReverseContiguousIterator() = default;

			constexpr ReverseContiguousIterator(const ReverseContiguousIterator<T, Cont>&) noexcept = default;
			constexpr auto operator=(const ReverseContiguousIterator<T, Cont>&) noexcept -> ReverseContiguousIterator& = default;
			constexpr ReverseContiguousIterator(ReverseContiguousIterator<T, Cont>&&) noexcept = default;
			constexpr auto operator=(ReverseContiguousIterator<T, Cont>&&) noexcept -> ReverseContiguousIterator& = default;

			constexpr auto operator==(const ReverseContiguousIterator<T, Cont> &it) const noexcept -> bool;
			constexpr auto operator<=>(const ReverseContiguousIterator<T, Cont> &it) const noexcept -> std::partial_ordering;

			constexpr auto operator++() noexcept -> ReverseContiguousIterator<T, Cont>&;
			constexpr auto operator--() noexcept -> ReverseContiguousIterator<T, Cont>&;
			constexpr auto operator++(int) noexcept -> ReverseContiguousIterator<T, Cont>;
			constexpr auto operator--(int) noexcept -> ReverseContiguousIterator<T, Cont>;

			constexpr auto operator+=(difference_type value) noexcept -> ReverseContiguousIterator<T, Cont>&;
			constexpr auto operator-=(difference_type value) noexcept -> ReverseContiguousIterator<T, Cont>&;

			constexpr auto operator+(difference_type value) const noexcept -> ReverseContiguousIterator<T, Cont>;
			friend constexpr auto operator+(difference_type value, ReverseContiguousIterator<T, Cont> it) noexcept -> ReverseContiguousIterator<T, Cont>;
			constexpr auto operator-(difference_type value) const noexcept -> ReverseContiguousIterator<T, Cont>;

			constexpr auto operator-(const ReverseContiguousIterator<T, Cont> &it) const noexcept -> difference_type;

			constexpr auto operator[](difference_type value) const noexcept -> reference;
			constexpr auto operator*() const noexcept -> reference;
			constexpr auto operator->() const noexcept -> pointer;


		protected:
			constexpr ReverseContiguousIterator(pointer ptr, const Container *container) noexcept;

		private:
			pointer m_ptr;
			const Container *m_container;
	};


	static_assert(std::contiguous_iterator<ContiguousIterator<int, int>>);
	static_assert(std::contiguous_iterator<ReverseContiguousIterator<int, int>>);

} // namespace flex

#include "flex/contiguousIterator.inl"
