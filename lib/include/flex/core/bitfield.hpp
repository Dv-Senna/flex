#pragma once

#include <ranges>
#include <string>
#include <print>

#include "flex/reflection/enums.hpp"


namespace flex {
	template <flex::enumeration T>
	class Bitfield final {
		public:
			using Type = std::underlying_type_t<T>;

			constexpr Bitfield() noexcept = default;
			constexpr ~Bitfield() = default;
			constexpr Bitfield(const Bitfield<T>&) noexcept = default;
			constexpr auto operator=(const Bitfield<T>&) noexcept -> Bitfield<T>& = default;
			constexpr Bitfield(Bitfield<T>&&) noexcept = default;
			constexpr auto operator=(Bitfield<T>&&) noexcept -> Bitfield<T>& = default;

			constexpr Bitfield(T value) noexcept : m_value {static_cast<Type> (value)} {}
			constexpr auto operator=(T value) noexcept -> Bitfield<T>& {
				m_value = static_cast<Type> (value);
				return *this;
			}

			constexpr auto operator==(const Bitfield<T>&) const noexcept -> bool = default;
			constexpr auto operator==(T value) const noexcept -> bool {return m_value == static_cast<Type> (value);}
			constexpr operator bool() const noexcept {return !!m_value;}

			constexpr auto operator&=(const Bitfield<T> &bitfield) noexcept -> Bitfield<T>& {m_value &= bitfield.m_value; return *this;}
			constexpr auto operator|=(const Bitfield<T> &bitfield) noexcept -> Bitfield<T>& {m_value |= bitfield.m_value; return *this;}
			constexpr auto operator^=(const Bitfield<T> &bitfield) noexcept -> Bitfield<T>& {m_value ^= bitfield.m_value; return *this;}

			constexpr auto operator&=(T value) noexcept -> Bitfield<T>& {m_value &= static_cast<Type> (value); return *this;}
			constexpr auto operator|=(T value) noexcept -> Bitfield<T>& {m_value |= static_cast<Type> (value); return *this;}
			constexpr auto operator^=(T value) noexcept -> Bitfield<T>& {m_value ^= static_cast<Type> (value); return *this;}

			constexpr auto operator&(const Bitfield<T> &bitfield) const noexcept -> Bitfield<T> {auto tmp {*this}; return tmp &= bitfield;}
			constexpr auto operator|(const Bitfield<T> &bitfield) const noexcept -> Bitfield<T> {auto tmp {*this}; return tmp |= bitfield;}
			constexpr auto operator^(const Bitfield<T> &bitfield) const noexcept -> Bitfield<T> {auto tmp {*this}; return tmp ^= bitfield;}

			constexpr auto operator&(T value) const noexcept -> Bitfield<T> {auto tmp {*this}; return tmp &= value;}
			constexpr auto operator|(T value) const noexcept -> Bitfield<T> {auto tmp {*this}; return tmp |= value;}
			constexpr auto operator^(T value) const noexcept -> Bitfield<T> {auto tmp {*this}; return tmp ^= value;}

			constexpr auto operator~() const noexcept -> Bitfield<T> {auto tmp {*this}; ~tmp.m_value; return tmp;}

			explicit constexpr operator Type() const noexcept {return m_value;}

		private:
			Type m_value;
	};


	template <flex::enumeration T>
	constexpr auto operator==(T value, const Bitfield<T> &bitfield) noexcept -> bool {return bitfield == value;}

	template <flex::enumeration T>
	constexpr auto operator&(T value, const Bitfield<T> &bitfield) noexcept -> Bitfield<T> {return bitfield & value;}
	template <flex::enumeration T>
	constexpr auto operator|(T value, const Bitfield<T> &bitfield) noexcept -> Bitfield<T> {return bitfield | value;}
	template <flex::enumeration T>
	constexpr auto operator^(T value, const Bitfield<T> &bitfield) noexcept -> Bitfield<T> {return bitfield ^ value;}

	template <flex::enumeration T>
	constexpr auto operator~(T value) noexcept -> Bitfield<T> {return ~Bitfield<T> {value};}


	template <flex::enumeration T>
	constexpr auto toString(const Bitfield<T> &bitfield) noexcept -> std::optional<std::string> {
		using Type = typename Bitfield<T>::Type;
		if (!bitfield)
			return std::nullopt;
		std::string result {};
		const auto size {
			sizeof(typename Bitfield<T>::Type) * 8
			- (std::is_unsigned_v<Type> ? 0 : 1)
		};
		for (const auto i : std::views::iota(0uz, size)) {
			const auto bit {static_cast<T> (static_cast<Type> (1) << i)};
			if (!(bitfield & bit))
				continue;

			if (!result.empty())
				result += " | ";
			result += flex::toString(bit).value_or("<invalid>");
		}
		return result;
	}

} // namespace flex


template <flex::scoped_enumeration T>
constexpr auto operator|(T lhs, T rhs) noexcept -> flex::Bitfield<T> {return flex::Bitfield<T> {lhs} | rhs;}

