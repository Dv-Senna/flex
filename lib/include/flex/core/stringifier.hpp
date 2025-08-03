#pragma once

#include <charconv>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "flex/core/errorType.hpp"
#include "flex/core/typeTraits.hpp"

namespace flex {
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
			do {
				buffer.resize(buffer.size() + chunkSize);
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
		constexpr auto operator()(flex::variant_of<T> auto&& value) const noexcept {
			return std::forward<decltype(value)> (value);
		}
	};

	static_assert(nonfailable_stringifyable<int>);
	static_assert(nonfailable_stringifyable<float>);
	static_assert(nonfailable_stringifyable<bool>);
	static_assert(nonfailable_stringifyable<char*>);
	static_assert(nonfailable_stringifyable<const char*>);
	static_assert(nonfailable_stringifyable<const char(&)[12]>);
	static_assert(nonfailable_stringifyable<std::string_view>);
	static_assert(nonfailable_stringifyable<std::string>);
}
