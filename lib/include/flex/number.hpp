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

			auto operator==(const Self&) const noexcept -> bool = default;
			auto operator<=>(const Self&) const noexcept = default;

			auto operator+=(const Self &self) noexcept -> Self& {m_value += self.m_value; return *this;}
			auto operator-=(const Self &self) noexcept -> Self& {m_value -= self.m_value; return *this;}
			auto operator*=(const Self &self) noexcept -> Self& {m_value *= self.m_value; return *this;}
			auto operator/=(const Self &self) noexcept -> Self& {m_value /= self.m_value; return *this;}

			auto operator+(const Self &self) const noexcept -> Self {return {m_value + self.m_value};}
			auto operator-(const Self &self) const noexcept -> Self {return {m_value - self.m_value};}
			auto operator*(const Self &self) const noexcept -> Self {return {m_value * self.m_value};}
			auto operator/(const Self &self) const noexcept -> Self {return {m_value / self.m_value};}

			auto operator%(const Self &self) const noexcept -> Self requires (std::is_integral_v<T>) {return {m_value % self.m_value};}

		private:
			T m_value;
	};

} // namespace flex
