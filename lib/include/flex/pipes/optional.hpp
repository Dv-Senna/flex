#pragma once

#include <charconv>
#include <concepts>
#include <cstring>
#include <format>
#include <locale>
#include <optional>
#include <type_traits>
#include <vector>

#include "flex/pipes/pipes.hpp"
#include "flex/typeTraits.hpp"


namespace flex {
	template <typename T>
	requires flex::optional<std::remove_cvref_t<T>>
	[[nodiscard]]
	constexpr auto flatten(T &&optional) -> std::optional<flex::fully_unwrap_optional_t<std::remove_cvref_t<T>>> {
		if constexpr (flex::optional<typename std::remove_cvref_t<T>::value_type>) {
			if (!optional)
				return std::nullopt;
			return flatten(*optional);
		}
		else
			return optional;
	}

} // namespace flex


namespace flex::pipes {
	template <typename Tag>
	struct PipeBase {
		template <typename ...Args>
		[[nodiscard]]
		constexpr auto operator()(Args &&...args) const noexcept {
			return Tag{std::forward<Args> (args)...};
		}
	};

	template <template <typename...> typename Tag>
	struct TemplatePipeBase {
		template <typename ...Args>
		[[nodiscard]]
		constexpr auto operator()(Args &&...args) const noexcept {
			return Tag{std::forward<Args> (args)...};
		}
	};


	template <flex::arithmetic T>
	struct __ToNumberTag {
		std::conditional_t<std::integral<T>, int, std::chars_format> args;
	};

	template <flex::arithmetic T>
	constexpr PipeBase<__ToNumberTag<T>> to_number {};

	template <typename T, flex::arithmetic Number, flex::string CleanT = std::remove_cvref_t<T>>
	[[nodiscard]]
	constexpr auto operator|(const std::optional<T> &optional, __ToNumberTag<Number> &&toNumberTag) noexcept
		-> std::optional<Number>
	{
		if (!optional)
			return std::nullopt;
		const char *start {};
		const char *end {};
		if constexpr (std::same_as<std::string_view, CleanT> || std::same_as<std::string, CleanT>) {
			start = optional->data();
			end = start + optional->size();
		}
		else {
			start = *optional;
			end = std::strlen(*optional);
		}
		Number number {};
		[[maybe_unused]]
		const auto [ptr, err] {std::from_chars(start, end, number, toNumberTag.args)};
		if (err != std::errc{})
			return std::nullopt;
		return number;
	}


	template <typename T>
	requires std::same_as<T, std::remove_cvref_t<T>>
	struct __ConvertToTag {};

	template <typename To>
	constexpr PipeBase<__ConvertToTag<To>> convert_to {};

	template <typename From, typename To, std::convertible_to<To> CleanFrom = std::remove_cvref_t<From>>
	[[nodiscard]]
	constexpr auto operator|(const std::optional<From> &optional, __ConvertToTag<To>&&) noexcept
		-> std::optional<To>
	{
		if (!optional)
			return std::nullopt;
		return static_cast<To> (*optional);
	}


	template <typename T>
	requires std::same_as<T, std::remove_cvref_t<T>>
	struct __ConstructToTag {};

	template <typename To>
	constexpr PipeBase<__ConstructToTag<To>> construct_to {};

	template <typename From, std::constructible_from<From> To>
	[[nodiscard]]
	constexpr auto operator|(const std::optional<From> &optional, __ConstructToTag<To>&&) noexcept {
		if (!optional)
			return std::nullopt;
		return To(*optional);
	};

} // namespace flex::pipes
