#pragma once

#include <cassert>
#include <iterator>
#include <type_traits>


namespace flex::containers {
	template <typename T, typename Container>
	class ContiguousIterator {
		friend Container;
		public:
			using value_type = std::remove_cv_t<T>;
			using element_type = T;
			using difference_type = std::ptrdiff_t;
			using reference = std::add_lvalue_reference_t<element_type>;
			using pointer = std::add_pointer_t<element_type>;
			using iterator_category = std::contiguous_iterator_tag;

			constexpr ContiguousIterator() noexcept = default;
			constexpr ~ContiguousIterator() = default;
			constexpr ContiguousIterator(const ContiguousIterator&) noexcept = default;
			constexpr auto operator=(const ContiguousIterator&) noexcept -> ContiguousIterator& = default;
			constexpr ContiguousIterator(ContiguousIterator&&) noexcept = default;
			constexpr auto operator=(ContiguousIterator&&) noexcept -> ContiguousIterator& = default;

			constexpr auto operator==(const ContiguousIterator&) const noexcept -> bool = default;
			constexpr auto operator<=>(const ContiguousIterator&) const noexcept = default;

			constexpr auto operator*() const noexcept -> reference {assert(m_data != nullptr); return *m_data;}
			constexpr auto operator->() const noexcept -> pointer {assert(m_data != nullptr); return m_data;}
			constexpr auto operator[](difference_type pos) const noexcept -> reference {
				assert(m_data != nullptr);
				return m_data[pos];
			}

			constexpr auto operator++() noexcept -> ContiguousIterator& {
				assert(m_data != nullptr);
				++m_data;
				return *this;
			}
			constexpr auto operator++(int) noexcept -> ContiguousIterator {auto tmp {*this}; ++(*this); return tmp;}
			constexpr auto operator--() noexcept -> ContiguousIterator& {
				assert(m_data != nullptr);
				--m_data;
				return *this;
			}
			constexpr auto operator--(int) noexcept -> ContiguousIterator {auto tmp {*this}; --(*this); return tmp;}

			constexpr auto operator+=(difference_type off) noexcept -> ContiguousIterator& {
				assert(m_data != nullptr);
				m_data += off;
				return *this;
			}
			constexpr auto operator-=(difference_type off) noexcept -> ContiguousIterator& {
				assert(m_data != nullptr);
				m_data -= off;
				return *this;
			}

			constexpr auto operator+(difference_type off) const noexcept -> ContiguousIterator {
				auto tmp {*this};
				return tmp += off;
			}
			constexpr auto operator-(difference_type off) const noexcept -> ContiguousIterator {
				auto tmp {*this};
				return tmp -= off;
			}
			constexpr auto operator-(const ContiguousIterator& other) const noexcept -> difference_type {
				assert(m_data != nullptr && other.m_data != nullptr);
				return m_data - other.m_data;
			}

			friend auto operator+(difference_type off, ContiguousIterator it) noexcept -> ContiguousIterator {
				return it += off;
			}


		private:
			constexpr ContiguousIterator(pointer data) noexcept : m_data {data} {}
			pointer m_data;
	};


	static_assert(std::contiguous_iterator<ContiguousIterator<int, void>>);
	static_assert(std::contiguous_iterator<ContiguousIterator<const int, void>>);
}
