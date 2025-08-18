#pragma once

#include <charconv>
#include <cstring>
#include <type_traits>

#include "flex/core/typeTraits.hpp"
#include "flex/pipes/pipes.hpp"


namespace flex::pipes {
	template <flex::arithmetic T>
	class ToNumberPipe final {
		static constexpr auto COND {std::integral<T>};
		using Args = std::conditional_t<COND, int, std::chars_format>;
		static constexpr auto DEFAULT_ARGS {flex::conditional_value_v<COND, 10, std::chars_format{}>};
		public:
			constexpr ToNumberPipe(Args args = DEFAULT_ARGS) noexcept : m_args {args} {}
			ToNumberPipe(const ToNumberPipe&) = delete;
			auto operator=(const ToNumberPipe&) -> ToNumberPipe& = delete;
			ToNumberPipe(ToNumberPipe&&) = delete;
			auto operator=(ToNumberPipe&&) -> ToNumberPipe& = delete;
			constexpr ~ToNumberPipe() = default;

			template <typename String>
			requires flex::string<std::remove_cvref_t<String>>
			[[nodiscard]]
			constexpr auto operator()(String &&string) noexcept -> std::optional<T> {
				using CleanString = std::remove_cvref_t<String>;
				const char *start {};
				const char *end {};
				if constexpr (std::same_as<std::string_view, CleanString> || std::same_as<std::string, CleanString>) {
					start = std::forward<String> (string).data();
					end = start + std::forward<String> (string).size();
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
				if (!std::forward<Optional> (optional))
					return std::nullopt;
				return (*this)(*std::forward<Optional> (optional));
			}

		private:
			Args m_args;
	};

	template <typename T>
	ToNumberPipe(T &&value) -> ToNumberPipe<T>;

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<ToNumberPipe<T>> to_number {};

} // namespace flex::pipes
