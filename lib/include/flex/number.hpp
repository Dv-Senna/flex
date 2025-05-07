#pragma once

#include "flex/typeTraits.hpp"


namespace flex {
	template <flex::arithmetic T>
	class Number {
		using Self = Number<T>;

		public:
			constexpr Number() noexcept = default;
			constexpr ~Number() = default;
			constexpr Number(T &&value) noexcept : m_value {value} {}

			constexpr Number(const Self&) noexcept = default;
			constexpr auto operator=(const Self&) noexcept -> Self& = default;
			constexpr Number(Self&&) noexcept = default;
			constexpr auto operator=(Self&&) noexcept -> Self& = default;

			constexpr auto operator==(const Self&) const noexcept -> bool = default;
			constexpr auto operator<=>(const Self&) const noexcept = default;

			constexpr auto operator+=(const Self &self) noexcept -> Self& {m_value += self.m_value; return *this;}
			constexpr auto operator-=(const Self &self) noexcept -> Self& {m_value -= self.m_value; return *this;}
			constexpr auto operator*=(const Self &self) noexcept -> Self& {m_value *= self.m_value; return *this;}
			constexpr auto operator/=(const Self &self) noexcept -> Self& {m_value /= self.m_value; return *this;}

			constexpr auto operator+(const Self &self) const noexcept -> Self {return {m_value + self.m_value};}
			constexpr auto operator-(const Self &self) const noexcept -> Self {return {m_value - self.m_value};}
			constexpr auto operator*(const Self &self) const noexcept -> Self {return {m_value * self.m_value};}
			constexpr auto operator/(const Self &self) const noexcept -> Self {return {m_value / self.m_value};}

			constexpr auto operator%(const Self &self) const noexcept -> Self requires (std::is_integral_v<T>) {return {m_value % self.m_value};}

			explicit constexpr operator T() const noexcept {return m_value;}


		private:
			T m_value;
	};

} // namespace flex
