#pragma once

#include <charconv>
#include <cstring>
#include <type_traits>

#include "flex/pipes/pipes.hpp"
#include "flex/typeTraits.hpp"


namespace flex::pipes {
	template <flex::arithmetic T>
	class ToNumberPipe final {
		static constexpr auto COND {std::integral<T>};
		using Args = std::conditional_t<COND, int, std::chars_format>;
		static constexpr auto DEFAULT_ARGS {flex::conditional_value_v<COND, 10, std::chars_format{}>};
		public:
			constexpr ToNumberPipe(Args args = DEFAULT_ARGS) noexcept : m_args {args} {}
			constexpr ~ToNumberPipe() = default;

			template <typename String>
			requires flex::string<std::remove_cvref_t<String>>
			[[nodiscard]]
			constexpr auto operator()(String &&string) noexcept -> std::optional<T> {
				using CleanString = std::remove_cvref_t<String>;
				const char *start {};
				const char *end {};
				if constexpr (std::same_as<std::string_view, CleanString> || std::same_as<std::string, CleanString>) {
					start = string.data();
					end = start + string.size();
				}
				else {
					start = string;
					end = std::strlen(string);
				}
				T number {};
				[[maybe_unused]]
				const auto [ptr, err] {std::from_chars(start, end, number, m_args)};
				if (err != std::errc{})
					return std::nullopt;
				return number;
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!optional)
					return std::nullopt;
				return (*this)(*optional);
			}

		private:
			Args m_args;
	};

	template <typename T>
	ToNumberPipe(T &&value) -> ToNumberPipe<T>;

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<ToNumberPipe<T>> to_number {};

} // namespace flex::pipes
