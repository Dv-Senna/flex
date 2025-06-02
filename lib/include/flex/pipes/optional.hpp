#pragma once

#include <charconv>
#include <concepts>
#include <format>
#include <locale>
#include <optional>
#include <vector>

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


	template <typename Callback>
	struct __TransformTag {
		Callback &&callback;
	};

	template <typename Callback>
	__TransformTag(Callback&&) -> __TransformTag<Callback>;

	constexpr TemplatePipeBase<__TransformTag> transform {};

	template <typename T, typename Callback>
	requires std::invocable<Callback, std::add_lvalue_reference_t<T>>
	[[nodiscard]]
	constexpr auto operator|(std::optional<T> &optional, __TransformTag<Callback> &&transformTag) noexcept
		-> std::optional<flex::fully_unwrap_optional_t<std::remove_cvref_t<decltype(transformTag.callback(*optional))>>>
	{
		auto callback {std::forward<Callback> (transformTag.callback)};
		using CallbackResult = std::remove_cvref_t<decltype(callback(*optional))>;
		using Res = flex::fully_unwrap_optional_t<CallbackResult>;
		if (!optional)
			return std::optional<Res> {std::nullopt};
		if constexpr (flex::optional<CallbackResult>)
			return flatten(callback(*optional));
		return callback(*optional);
	}

	template <typename T, typename Callback>
	requires std::invocable<Callback, std::add_lvalue_reference_t<std::add_const_t<T>>>
	[[nodiscard]]
	constexpr auto operator|(const std::optional<T> &optional, __TransformTag<Callback> &&transformTag) noexcept
		-> std::optional<flex::fully_unwrap_optional_t<std::remove_cvref_t<decltype(transformTag.callback(*optional))>>>
	{
		auto callback {std::forward<Callback> (transformTag.callback)};
		using CallbackResult = std::remove_cvref_t<decltype(callback(*optional))>;
		using Res = flex::fully_unwrap_optional_t<CallbackResult>;
		if (!optional)
			return std::optional<Res> {std::nullopt};
		if constexpr (flex::optional<CallbackResult>)
			return flatten(callback(*optional));
		return callback(*optional);
	}


	template <typename T>
	struct __ValueOrTag {
		T &&value;
	};

	template <typename T>
	__ValueOrTag(T&&) -> __ValueOrTag<T>;

	constexpr TemplatePipeBase<__ValueOrTag> value_or {};

	template <typename T, typename ValueOr,
		typename CleanT = std::remove_cvref_t<T>,
		typename CleanValueOr = std::remove_cvref_t<ValueOr>
	>
	requires (
		std::convertible_to<CleanValueOr, CleanT>
		|| std::constructible_from<CleanT, CleanValueOr>
	)
	[[nodiscard]]
	constexpr auto operator|(const std::optional<T> &optional, __ValueOrTag<ValueOr> &&valueOrTag) noexcept
		-> CleanT
	{
		if (!optional) {
			if constexpr (std::convertible_to<CleanValueOr, CleanT>)
				return static_cast<CleanT> (valueOrTag.value);
			else
				return CleanT(valueOrTag.value);
		}
		return *optional;
	}


	template <typename ...Args>
	struct __ToStringTag {
		std::optional<std::locale> locale;
		std::tuple<Args&&...> args;
	};

	constexpr TemplatePipeBase<__ToStringTag> to_string {};

	template <flex::stringifyable T, typename ...Args, typename CleanT = std::remove_cvref_t<T>>
	[[nodiscard]]
	constexpr auto operator|(const std::optional<T> &optional, __ToStringTag<Args...> &&toStringTag) noexcept
		-> std::optional<std::string>
	{
		if (!optional)
			return std::nullopt;
		if constexpr (flex::arithmetic<CleanT>) {
			if (!toStringTag.locale) {
				constexpr std::size_t chunkSize {16};
				std::vector<char> buffer {};
				std::errc err {};
				char *ptr {nullptr};
				do {
					buffer.resize(buffer.size() + chunkSize);
					auto &&func {[&](auto&& ...args) {
						return std::to_chars(buffer.data(), buffer.data() + buffer.size(), *optional, args...);
					}};
					const auto [_ptr, _err] {std::apply(func, toStringTag.args)};
					err = _err;
					ptr = _ptr;
				} while (err == std::errc::value_too_large);
				return std::string{buffer.data(), ptr};
			}
			else
				return std::format(*toStringTag.locale, "{}", *optional);
		}
		else if constexpr (flex::string<CleanT>)
			return std::string{*optional};
		else if constexpr (std::same_as<CleanT, char>) {
			if (!toStringTag.locale) {
				if (*optional)
					return "true";
				else
					return "false";
			}
			else
				return std::format(*toStringTag.locale, "{}", *optional);
		}
	}


} // namespace flex::pipes
