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
