#pragma once

#include <charconv>
#include <format>
#include <locale>
#include <vector>

#include "flex/core/typeTraits.hpp"
#include "flex/pipes/pipes.hpp"
#include "flex/core/stringifier.hpp"


namespace flex::pipes {
	class ToStringPipe {
		public:
			constexpr ToStringPipe(std::optional<std::locale> locale = std::nullopt) noexcept :
				m_locale {std::move(locale)}
			{}
			ToStringPipe(const ToStringPipe&) = delete;
			auto operator=(const ToStringPipe&) -> ToStringPipe& = delete;
			ToStringPipe(ToStringPipe&&) = delete;
			auto operator=(ToStringPipe&&) -> ToStringPipe& = delete;
			constexpr ~ToStringPipe() = default;

			template <typename Stringifyable>
			[[nodiscard]]
			constexpr auto operator()(flex::stringifyable auto &&value) noexcept {
				return flex::toString(std::forward<decltype(value)> (value));
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				using Result = decltype((*this)(*std::forward<Optional> (optional)));
				using OptionalResult = std::conditional_t<flex::optional<Result>, Result, std::optional<Result>>;
				if (!std::forward<Optional> (optional))
					return OptionalResult{std::nullopt};
				return OptionalResult{(*this)(*std::forward<Optional> (optional))};
			}

		private:
			std::optional<std::locale> m_locale;
	};


	constexpr flex::pipes::PipeAdaptator<ToStringPipe> to_string {};

} // namespace flex::pipes
