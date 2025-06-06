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

			template <typename Optional,
				typename String = std::remove_cvref_t<typename std::remove_cvref_t<Optional>::value_type>
			>
			requires flex::optional<std::remove_cvref_t<Optional>>
				&& flex::string<String>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept
				-> std::optional<T>
			{
				if (!optional)
					return std::nullopt;
				const char *start {};
				const char *end {};
				if constexpr (std::same_as<std::string_view, String> || std::same_as<std::string, String>) {
					start = optional->data();
					end = start + optional->size();
				}
				else {
					start = *optional;
					end = std::strlen(*optional);
				}
				T number {};
				[[maybe_unused]]
				const auto [ptr, err] {std::from_chars(start, end, number, m_args)};
				if (err != std::errc{})
					return std::nullopt;
				return number;
			}

		private:
			Args m_args;
	};

	template <typename T>
	ToNumberPipe(T &&value) -> ToNumberPipe<T>;

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<ToNumberPipe<T>> to_number {};

} // namespace flex::pipes
