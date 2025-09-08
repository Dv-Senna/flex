#pragma once

#include <cassert>
#include <optional>
#include <version>

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
	#include <expected>
#endif

#include "flex/core/typeTraits.hpp"


namespace flex {
	template <typename T>
	struct wrapper_trait;

	namespace internals {
		template <typename T>
		concept generatable_wrapper_trait = std::same_as<std::remove_cvref_t<T>, T>
			&& (
				requires(T v) {{!v} -> std::same_as<bool>;}
				|| requires(T v) {{v.has_value()} -> std::same_as<bool>;}
				|| requires(T v) {{v.hasValue()} -> std::same_as<bool>;}
			)
			&& (
				requires(T v) {*v;}
				|| requires(T v) {v.value();}
				|| requires(T v) {v.getValue();}
			);
	}

	template <typename T>
	struct wrapper_trait<std::optional<T>> {
		using type = std::optional<T>;
		using value_type = std::optional<T>::value_type;
		using error_type = void;

		static constexpr auto has_value(flex::forward_of<type> auto&& optional) noexcept -> bool {
			return !!std::forward<decltype(optional)> (optional);
		}

		static constexpr auto value(flex::forward_of<type> auto&& optional)
			noexcept
			-> decltype(*std::forward<decltype(optional)> (optional))
		{
			assert(!!std::forward<decltype(optional)> (optional));
			return *std::forward<decltype(optional)> (optional);
		}

		static constexpr auto unchecked_value(flex::forward_of<type> auto&& optional)
			noexcept
			-> decltype(*std::forward<decltype(optional)> (optional))
		{
			return *std::forward<decltype(optional)> (optional);
		}

		static constexpr auto value_or(
			flex::forward_of<type> auto&& optional,
			flex::forward_of<value_type> auto&& value
		)
			noexcept
			-> decltype(std::forward<decltype(optional)>.value_or(std::forward<decltype(value)> (value)))
		{
			return std::forward<decltype(optional)> (optional).value_or(std::forward<decltype(value)> (value));
		}

		static constexpr auto transform(
			flex::forward_of<type> auto&& optional,
			flex::noexcept_predicate<value_type> auto&& predicate
		)
			noexcept
			-> decltype(std::forward<decltype(optional)>.transform(std::forward<decltype(predicate)> (predicate)))
		{
			return std::forward<decltype(optional)> (optional).transform(std::forward<decltype(predicate)> (predicate));
		}

		static constexpr auto and_then(
			flex::forward_of<type> auto&& optional,
			flex::noexcept_predicate<> auto&& predicate
		)
			noexcept
			-> decltype(std::forward<decltype(optional)>.and_then(std::forward<decltype(predicate)> (predicate)))
		{
			return std::forward<decltype(optional)>.and_then(std::forward<decltype(predicate)> (predicate));
		}

		static constexpr auto or_else(
			flex::forward_of<type> auto&& optional,
			flex::noexcept_predicate_with_strict_return<type> auto&& predicate
		)
			noexcept
			-> decltype(std::forward<decltype(optional)>.or_else(std::forward<decltype(predicate)> (predicate)))
		{
			return std::forward<decltype(optional)>.or_else(std::forward<decltype(predicate)> (predicate));
		}
	};


#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
	template <typename T, typename Err>
	struct wrapper_trait<std::expected<T, Err>> {
		using type = std::expected<T, Err>;
		using value_type = std::expected<T, Err>::value_type;
		using error_type = std::expected<T, Err>::error_type;

		static constexpr auto has_value(flex::forward_of<type> auto&& expected) noexcept -> bool {
			return !!std::forward<decltype(expected)> (expected);
		}

		static constexpr auto value(flex::forward_of<type> auto&& expected)
			noexcept
			-> decltype(*std::forward<decltype(expected)> (expected))
		{
			assert(!!std::forward<decltype(expected)> (expected));
			return *std::forward<decltype(expected)> (expected);
		}

		static constexpr auto unchecked_value(flex::forward_of<type> auto&& expected)
			noexcept
			-> decltype(*std::forward<decltype(expected)> (expected))
		{
			return *std::forward<decltype(expected)> (expected);
		}

		static constexpr auto error(flex::forward_of<type> auto&& expected)
			noexcept
			-> decltype(std::forward<decltype(expected)> (expected).error())
		{
			assert(!std::forward<decltype(expected)> (expected));
			return std::forward<decltype(expected)> (expected).error();
		}

		static constexpr auto unchecked_error(flex::forward_of<type> auto&& expected)
			noexcept
			-> decltype(std::forward<decltype(expected)> (expected).error())
		{
			return std::forward<decltype(expected)> (expected).error();
		}

		static constexpr auto value_or(
			flex::forward_of<type> auto&& expected,
			flex::forward_of<value_type> auto&& value
		)
			noexcept
			-> decltype(std::forward<decltype(expected)>.value_or(std::forward<decltype(value)> (value)))
		{
			return std::forward<decltype(expected)> (expected).value_or(std::forward<decltype(value)> (value));
		}

		static constexpr auto error_or(
			flex::forward_of<type> auto&& expected,
			flex::forward_of<error_type> auto&& value
		)
			noexcept
			-> decltype(std::forward<decltype(expected)>.error_or(std::forward<decltype(value)> (value)))
		{
			return std::forward<decltype(expected)> (expected).error_or(std::forward<decltype(value)> (value));
		}

		static constexpr auto transform(
			flex::forward_of<type> auto&& expected,
			flex::noexcept_predicate<value_type> auto&& predicate
		)
			noexcept
			-> decltype(std::forward<decltype(expected)>.transform(std::forward<decltype(predicate)> (predicate)))
		{
			return std::forward<decltype(expected)> (expected).transform(std::forward<decltype(predicate)> (predicate));
		}

		static constexpr auto transform_error(
			flex::forward_of<type> auto&& expected,
			flex::noexcept_predicate<error_type> auto&& predicate
		)
			noexcept
			-> decltype(std::forward<decltype(expected)>.transform_error(std::forward<decltype(predicate)> (predicate)))
		{
			return std::forward<decltype(expected)> (expected).transform_error(
				std::forward<decltype(predicate)> (predicate)
			);
		}

		static constexpr auto and_then(
			flex::forward_of<type> auto&& expected,
			flex::noexcept_predicate<> auto&& predicate
		)
			noexcept
			-> decltype(std::forward<decltype(expected)>.and_then(std::forward<decltype(predicate)> (predicate)))
		{
			return std::forward<decltype(expected)>.and_then(std::forward<decltype(predicate)> (predicate));
		}

		static constexpr auto or_else(
			flex::forward_of<type> auto&& expected,
			flex::noexcept_predicate_with_strict_return<type> auto&& predicate
		)
			noexcept
			-> decltype(std::forward<decltype(expected)>.or_else(std::forward<decltype(predicate)> (predicate)))
		{
			return std::forward<decltype(expected)>.or_else(std::forward<decltype(predicate)> (predicate));
		}
	};
#endif


	template <internals::generatable_wrapper_trait T>
	struct wrapper_trait<T> {
		using type = T;
		using value_type = ;

		private:
			template <typename U>
			struct value_type_from_method_provider;

			template <typename U>
			requires (requires(U v) {*v;})
			struct value_type_from_method_provider<U> : flex::type_constant<decltype(*std::declval<U> ())> {};
			template <typename U>
			requires (requires(U v) {v.value();} && !requires(U v) {*v;})
			struct value_type_from_method_provider<U> : flex::type_constant<decltype(std::declval<U> ().value())> {};
			template <typename U>
			requires (requires(U v) {v.getValue();} && !requires(U v) {v.value();} && !requires(U v) {*v;})
			struct value_type_from_method_provider<U> : flex::type_constant<decltype(std::declval<U> ().getValue())> {};

			template <typename U>
			struct value_type_provider : flex::type_constant<typename value_type_from_method_provider<U>::type> {};
			template <typename U>
			requires (requires {typename U::value_type;})
			struct value_type_provider<U> : flex::type_constant<typename U::value_type> {};
			template <typename U>
			requires (requires {typename U::ValueType;} && !requires {typename U::value_type;})
			struct value_type_provider<U> : flex::type_constant<typename U::ValueType> {};
	};
}
