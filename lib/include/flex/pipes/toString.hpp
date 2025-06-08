#pragma once

#include <charconv>
#include <format>
#include <locale>
#include <vector>

#include "flex/pipes/pipes.hpp"
#include "flex/reflection/enums.hpp"
#include "flex/typeTraits.hpp"


namespace flex::pipes {
	class ToStringPipe {
		public:
			constexpr ToStringPipe(std::optional<std::locale> locale = std::nullopt) noexcept : m_locale {locale} {}
			constexpr ~ToStringPipe() = default;

			template <typename Stringifyable>
			[[nodiscard]]
			constexpr auto operator()(Stringifyable &&stringifyable) noexcept {
				using T = std::remove_cvref_t<Stringifyable>;
				if constexpr (flex::arithmetic<T>) {
					if (!m_locale) {
						constexpr std::size_t chunkSize {16};
						std::vector<char> buffer {};
						std::errc err {};
						char *ptr {nullptr};
						do {
							buffer.resize(buffer.size() + chunkSize);
							const auto [_ptr, _err] {std::to_chars(buffer.data(), buffer.data() + buffer.size(), stringifyable)};
							err = _err;
							ptr = _ptr;
						} while (err == std::errc::value_too_large);
						return std::string{buffer.data(), ptr};
					}
					else
						return std::format(*m_locale, "{}", stringifyable);
				}
				else if constexpr (flex::string<T>)
					return std::string{stringifyable};
				else if constexpr (std::same_as<T, char>) {
					if (!m_locale) {
						if (stringifyable)
							return "true";
						else
							return "false";
					}
					else
						return std::format(*m_locale, "{}", stringifyable);
				}
				else
					return flex::toString(stringifyable);
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				using Result = decltype((*this)(*optional));
				using OptionalResult = std::conditional_t<flex::optional<Result>, Result, std::optional<Result>>;
				if (!optional)
					return OptionalResult{std::nullopt};
				return OptionalResult{(*this)(*optional)};
			}

		private:
			std::optional<std::locale> m_locale;
	};


	constexpr flex::pipes::PipeAdaptator<ToStringPipe> to_string {};

} // namespace flex::pipes
