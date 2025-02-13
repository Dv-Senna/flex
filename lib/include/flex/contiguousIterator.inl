#pragma once

#include "flex/contiguousIterator.hpp"

#include <limits>

#include "flex/config.hpp"


namespace flex {
	template <typename T, typename Cont>
	constexpr ContiguousIterator<T, Cont>::ContiguousIterator(PointerType ptr, const Container *container) noexcept :
		m_ptr {ptr},
		m_container {container}
	{

	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator==(const ContiguousIterator<T, Cont> &it) const noexcept -> bool {
		return m_container == it.m_container && m_ptr == it.m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator<=>(const ContiguousIterator<T, Cont> &it) const noexcept -> std::partial_ordering {
		if (m_container != it.m_container)
			return std::partial_ordering::unordered;
		return m_ptr <=> it.m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator++() noexcept -> ContiguousIterator<T, Cont>& {
		++m_ptr;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator--() noexcept -> ContiguousIterator<T, Cont>& {
		--m_ptr;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator++(int) noexcept -> ContiguousIterator<T, Cont> {
		auto tmp {*this};
		++(*this);
		return tmp;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator--(int) noexcept -> ContiguousIterator<T, Cont> {
		auto tmp {*this};
		--(*this);
		return tmp;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator+=(ArithmeticType value) noexcept -> ContiguousIterator<T, Cont>& {
		m_ptr += value;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator-=(ArithmeticType value) noexcept -> ContiguousIterator<T, Cont>& {
		m_ptr -= value;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator+(ArithmeticType value) const noexcept -> ContiguousIterator<T, Cont> {
		auto tmp {*this};
		return tmp += value;
	}

	template <typename T, typename Cont>
	constexpr auto operator+(typename ContiguousIterator<T, Cont>::ArithmeticType value, ContiguousIterator<T, Cont> it) noexcept -> ContiguousIterator<T, Cont> {
		return it += value;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator-(ArithmeticType value) const noexcept -> ContiguousIterator<T, Cont> {
		auto tmp {*this};
		return tmp -= value;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator-(const ContiguousIterator<T, Cont> &it) const noexcept -> ArithmeticType {
		if constexpr (flex::config::BUILD_TYPE == flex::BuildType::eDebug) {
			if (m_container != it.m_container)
				return std::numeric_limits<ArithmeticType>::max();
		}
		return m_ptr - it.m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator[](ArithmeticType value) const noexcept -> ReferenceType {
		return *(m_ptr + value);
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator*() const noexcept -> ReferenceType {
		return *m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ContiguousIterator<T, Cont>::operator->() const noexcept -> PointerType {
		return m_ptr;
	}




	template <typename T, typename Cont>
	constexpr ReverseContiguousIterator<T, Cont>::ReverseContiguousIterator(PointerType ptr, const Container *container) noexcept :
		m_ptr {ptr},
		m_container {container}
	{

	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator==(const ReverseContiguousIterator<T, Cont> &it) const noexcept -> bool {
		return m_container == it.m_container && m_ptr == it.m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator<=>(const ReverseContiguousIterator<T, Cont> &it) const noexcept -> std::partial_ordering {
		if (m_container != it.m_container)
			return std::partial_ordering::unordered;
		return it.m_ptr <=> m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator++() noexcept -> ReverseContiguousIterator<T, Cont>& {
		--m_ptr;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator--() noexcept -> ReverseContiguousIterator<T, Cont>& {
		++m_ptr;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator++(int) noexcept -> ReverseContiguousIterator<T, Cont> {
		auto tmp {*this};
		++(*this);
		return tmp;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator--(int) noexcept -> ReverseContiguousIterator<T, Cont> {
		auto tmp {*this};
		--(*this);
		return tmp;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator+=(ArithmeticType value) noexcept -> ReverseContiguousIterator<T, Cont>& {
		m_ptr -= value;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator-=(ArithmeticType value) noexcept -> ReverseContiguousIterator<T, Cont>& {
		m_ptr += value;
		return *this;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator+(ArithmeticType value) const noexcept -> ReverseContiguousIterator<T, Cont> {
		auto tmp {*this};
		return tmp += value;
	}

	template <typename T, typename Cont>
	constexpr auto operator+(typename ReverseContiguousIterator<T, Cont>::ArithmeticType value, ReverseContiguousIterator<T, Cont> it) noexcept -> ReverseContiguousIterator<T, Cont> {
		return it += value;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator-(ArithmeticType value) const noexcept -> ReverseContiguousIterator<T, Cont> {
		auto tmp {*this};
		return tmp -= value;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator-(const ReverseContiguousIterator<T, Cont> &it) const noexcept -> ArithmeticType {
		if constexpr (flex::config::BUILD_TYPE == flex::BuildType::eDebug) {
			if (m_container != it.m_container)
				return std::numeric_limits<ArithmeticType>::max();
		}
		return m_ptr - it.m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator[](ArithmeticType value) const noexcept -> ReferenceType {
		return *(m_ptr + value);
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator*() const noexcept -> ReferenceType {
		return *m_ptr;
	}

	template <typename T, typename Cont>
	constexpr auto ReverseContiguousIterator<T, Cont>::operator->() const noexcept -> PointerType {
		return m_ptr;
	}

} // namespace flex
