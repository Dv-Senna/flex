#pragma once

#include <compare>
#include <type_traits>

#include "flex/iterator.hpp"


namespace flex {
	template <typename T, typename Cont>
	class ContiguousIterator {
		using Container = std::remove_cvref_t<Cont>;
		friend Container;

		public:
			static constexpr auto ITERATOR_CATEGORY {flex::IteratorCategory::eContiguous};
			using ValueType = std::remove_reference_t<T>;
			using ArithmeticType = std::ptrdiff_t;
			using ReferenceType = ValueType&;
			using PointerType = ValueType*;

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

			constexpr auto operator+=(ArithmeticType value) noexcept -> ContiguousIterator<T, Cont>&;
			constexpr auto operator-=(ArithmeticType value) noexcept -> ContiguousIterator<T, Cont>&;

			constexpr auto operator+(ArithmeticType value) const noexcept -> ContiguousIterator<T, Cont>;
			constexpr auto operator-(ArithmeticType value) const noexcept -> ContiguousIterator<T, Cont>;

			constexpr auto operator-(const ContiguousIterator<T, Cont> &it) const noexcept -> ArithmeticType;

			constexpr auto operator[](ArithmeticType value) const noexcept -> ReferenceType;
			constexpr auto operator*() const noexcept -> ReferenceType;
			constexpr auto operator->() const noexcept -> PointerType;


		protected:
			constexpr ContiguousIterator(PointerType ptr, const Container *container) noexcept;

		private:
			PointerType m_ptr;
			const Container *m_container;
	};

	template <typename T, typename Cont>
	constexpr auto operator+(typename ContiguousIterator<T, Cont>::difference_type value, ContiguousIterator<T, Cont> it) noexcept -> ContiguousIterator<T, Cont>;


	template <typename T, typename Cont>
	class ReverseContiguousIterator {
		using Container = std::remove_cvref_t<Cont>;
		friend Container;

		public:
			static constexpr auto ITERATOR_CATEGORY {flex::IteratorCategory::eContiguous};
			using ValueType = std::remove_reference_t<T>;
			using ArithmeticType = std::ptrdiff_t;
			using PointerType = ValueType*;
			using ReferenceType = ValueType&;

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

			constexpr auto operator+=(ArithmeticType value) noexcept -> ReverseContiguousIterator<T, Cont>&;
			constexpr auto operator-=(ArithmeticType value) noexcept -> ReverseContiguousIterator<T, Cont>&;

			constexpr auto operator+(ArithmeticType value) const noexcept -> ReverseContiguousIterator<T, Cont>;
			constexpr auto operator-(ArithmeticType value) const noexcept -> ReverseContiguousIterator<T, Cont>;

			constexpr auto operator-(const ReverseContiguousIterator<T, Cont> &it) const noexcept -> ArithmeticType;

			constexpr auto operator[](ArithmeticType value) const noexcept -> ReferenceType;
			constexpr auto operator*() const noexcept -> ReferenceType;
			constexpr auto operator->() const noexcept -> PointerType;


		protected:
			constexpr ReverseContiguousIterator(PointerType ptr, const Container *container) noexcept;

		private:
			PointerType m_ptr;
			const Container *m_container;
	};

	template <typename T, typename Cont>
	constexpr auto operator+(typename ReverseContiguousIterator<T, Cont>::difference_type value, ReverseContiguousIterator<T, Cont> it) noexcept -> ReverseContiguousIterator<T, Cont>;


} // namespace flex

#include "flex/contiguousIterator.inl"
