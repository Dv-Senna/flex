#pragma once

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "flex/core/comptime.hpp"
#include "flex/core/errorType.hpp"
#include "flex/core/typeTraits.hpp"

namespace flex {
	struct StringifyStyle {
		bool prettify {false};
		std::uint32_t indentSize {4};
		std::uint32_t currentIndent {0};
	};

	constexpr auto processStringifyStyle(flex::StringifyStyle style) {
		if (!style.prettify)
			return style;
		style.currentIndent += style.indentSize;
		return style;
	};

	template <typename T>
	struct Stringifier;

	template <typename T>
	concept nonfailable_stringifyable = requires (T value, const Stringifier<std::remove_reference_t<T>> stringifier) {
		Stringifier<std::remove_reference_t<T>> {};
		{stringifier(value)} -> flex::string;
	};

	template <typename T>
	concept failable_stringifyable = requires (T value, const Stringifier<std::remove_reference_t<T>> stringifier) {
		Stringifier<std::remove_reference_t<T>> {};
		{stringifier(value)} -> flex::error_type;
	}
		&& flex::string<flex::error_type_value_t<decltype(
			Stringifier<std::remove_reference_t<T>> {}(std::declval<T> ())
		)>>;

	template <typename T>
	concept stringifyable = nonfailable_stringifyable<T> || failable_stringifyable<T>;

	template <typename T, typename ...Args>
	concept stringifyable_with = stringifyable<T>
		&& requires(T value, const Stringifier<std::remove_cvref_t<T>> stringifier, Args... args) {
			{stringifier(value, args...)} -> std::same_as<decltype(stringifier(value))>;
		};


	[[nodiscard("Converting a value to a string can be a slow operation. Don't do it if you don't use its result")]]
	constexpr auto toString(stringifyable auto&& value, auto&&... args) noexcept {
		return Stringifier<std::remove_cvref_t<decltype(value)>> {}(
			std::forward<decltype(value)> (value),
			std::forward<decltype(args)> (args)...
		);
	}

	template <flex::arithmetic T>
	struct Stringifier<T> {
		constexpr auto operator()(T value) const noexcept -> std::string {
			constexpr std::size_t chunkSize {16};
			std::vector<char> buffer {};
			std::errc err {};
			char *ptr {nullptr};
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while)
			do {
				buffer.resize(buffer.size() + chunkSize);
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
				const auto [_ptr, _err] {std::to_chars(buffer.data(), buffer.data() + buffer.size(), value)};
				err = _err;
				ptr = _ptr;
			} while (err == std::errc::value_too_large);
			return std::string{buffer.data(), ptr};
		}
	};

	template <>
	struct Stringifier<bool> {
		constexpr auto operator()(bool value) const noexcept -> std::string_view {
			using namespace std::string_view_literals;
			return value ? "true"sv : "false"sv;
		}
	};

	template <flex::string T>
	struct Stringifier<T> {
		constexpr auto operator()(flex::forward_of<T> auto&& value) const noexcept {
			return static_cast<std::decay_t<decltype(value)>> (std::forward<decltype(value)> (value));
		}
	};

	template <stringifyable T>
	struct Stringifier<std::reference_wrapper<T>> {
		constexpr auto operator()(std::reference_wrapper<T> ref, auto&&... args) const noexcept {
			return flex::toString(ref.get(), std::forward<decltype(args)> (args)...);
		}
	};


	namespace internals {
		constexpr auto reflectedValueToString(
			auto&& value, const StringifyStyle& style
		) -> std::string {
			using Member = std::remove_reference_t<decltype(value)>;

			if constexpr (flex::formattable<Member>)
				return std::format("{}", std::forward<decltype(value)> (value));
			else if constexpr (flex::failable_stringifyable<Member>) {
				using ErrorTraits = flex::error_type_traits<decltype(
					flex::toString(std::forward<decltype(value)> (value))
				)>;
				using ValueType = typename ErrorTraits::ValueType;
				if constexpr (flex::stringifyable_with<Member, flex::StringifyStyle>) {
					return ErrorTraits::getValueOr(
						flex::toString(
							std::forward<decltype(value)> (value), flex::processStringifyStyle(style)
						), ValueType{"<to-string-error>"}
					);
				}
				else {
					return ErrorTraits::getValueOr(
						flex::toString(std::forward<decltype(value)> (value)), ValueType{"<to-string-error>"}
					);
				}
			}
			else if constexpr (flex::nonfailable_stringifyable<Member>) {
				if constexpr (flex::stringifyable_with<Member, flex::StringifyStyle>)
					return flex::toString(std::forward<decltype(value)> (value), flex::processStringifyStyle(style));
				else
					return flex::toString(std::forward<decltype(value)> (value));
			}
			else
				flex::comptimeErrorWithTypes<Member> ("Type can't be made into a string type");
			return "";
		}
	}

	template <std::ranges::input_range T>
	requires (!flex::string<T> && stringifyable<std::ranges::range_value_t<T>>)
	struct Stringifier<T> {
		constexpr auto operator()(flex::forward_of<T> auto&& range, const StringifyStyle& style = {}) const noexcept {
			std::string_view prefix {};
			std::string indent {};
			std::string result {"["};
			if (style.prettify) {
				prefix = "\n";
				indent = std::string(style.currentIndent + style.indentSize, ' ');
			}

			std::ranges::for_each(range, [&result, &prefix, &indent, &style](auto&& value) noexcept {
				const std::string valueAsString {internals::reflectedValueToString(
					std::forward<decltype(value)> (value), style
				)};
				result += std::format("{}{}{}", prefix, indent, valueAsString);
				if (style.prettify)
					prefix = ",\n";
				else
					prefix = ",";
			});
			if (style.prettify) {
				result.push_back('\n');
				result += std::string(style.currentIndent, ' ');
			}
			result.push_back(']');
			return result;
		}
	};

	static_assert(nonfailable_stringifyable<int>);
	static_assert(nonfailable_stringifyable<float>);
	static_assert(nonfailable_stringifyable<bool>);
	static_assert(nonfailable_stringifyable<char*>);
	static_assert(nonfailable_stringifyable<const char*>);
// ENOLINTNEXTLINE
	static_assert(nonfailable_stringifyable<const char(&)[12]>);
	static_assert(nonfailable_stringifyable<std::string_view>);
	static_assert(nonfailable_stringifyable<std::string>);
}
