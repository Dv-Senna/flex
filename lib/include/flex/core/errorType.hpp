#pragma once

#include <optional>
#include <type_traits>

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
	#include <expected>
#endif

#include "flex/core/typeTraits.hpp"


namespace flex {
	namespace internals {
		template <typename T>
		concept autogen_error_type = requires(const T cv) {
			typename T::value_type;
			{*cv} -> flex::similar_to<typename T::value_type>;
			{!cv} -> flex::similar_to<bool>;
		}
			&& std::movable<T>
			&& std::is_default_constructible_v<std::remove_cvref_t<T>>
			&& std::same_as<T, std::remove_cvref_t<T>>;


		template <typename T>
		concept autogen_error_type_with_error = requires(const T cv) {
			typename T::error_type;
			{cv.error()} -> flex::similar_to<typename T::error_type>;
		}
			&& autogen_error_type<T>;

	}


	template <typename T>
	requires std::same_as<T, std::remove_cvref_t<T>>
	struct error_type_traits;

	namespace internals {
		template <typename Traits>
		concept valid_error_type_traits = requires(std::add_const_t<typename Traits::type> cv) {
				typename Traits::type;
				typename Traits::value_type;
				typename Traits::error_type;

				{Traits::hasValue(cv)} -> flex::similar_to<bool>;
				noexcept(Traits::hasValue(cv));
		}
			&& (
				std::is_void_v<typename Traits::value_type>
				|| requires(std::add_const_t<typename Traits::type> cv)
			{
				{Traits::getValue(cv)} -> flex::similar_to<typename Traits::value_type>;
				noexcept(Traits::getValue(cv));
			}
		);

		template <typename Traits>
		concept valid_error_type_traits_without_error = valid_error_type_traits<Traits>
			&& std::is_void_v<typename Traits::error_type>
			&& !std::is_void_v<typename Traits::value_type>;

		template <typename Traits>
		concept valid_error_type_traits_with_error = internals::valid_error_type_traits<Traits>
			&& !std::is_void_v<typename Traits::error_type>
			&& requires(std::add_const_t<typename Traits::type> cv)
		{
			{Traits::getError(cv)} -> flex::similar_to<typename Traits::error_type>;
		};

	}

	template <typename T>
	concept error_type_without_error = internals::valid_error_type_traits_without_error<
		error_type_traits<std::remove_cvref_t<T>>
	>;

	template <typename T>
	concept error_type_with_error = internals::valid_error_type_traits_with_error<
		error_type_traits<std::remove_cvref_t<T>>
	>;

	template <typename T>
	concept error_type = error_type_without_error<T> || error_type_with_error<T>;

	template <typename T>
	concept error_type_with_value = error_type<T> && !std::is_void_v<
		typename error_type_traits<std::remove_cvref_t<T>>::value_type
	>;

	template <typename T>
	concept error_type_without_value = error_type<T> && !error_type_with_value<T>;


	template <error_type T>
	using error_type_value_t = typename error_type_traits<T>::value_type;

	template <error_type_with_error T>
	using error_type_error_t = typename error_type_traits<T>::error_type;


	template <internals::autogen_error_type T>
	struct error_type_traits<T> {
		using type = T;
		using value_type = typename T::value_type;
		using error_type = void;

		[[nodiscard]]
		static constexpr auto hasValue(flex::forward_of<type> auto&& instance) noexcept -> bool {
			return !!std::forward<decltype(instance)> (instance);
		}
		[[nodiscard]]
		static constexpr auto getValue(flex::forward_of<type> auto&& instance) noexcept
			-> std::conditional_t<std::is_lvalue_reference_v<decltype(instance)>,
				std::add_lvalue_reference_t<value_type>, std::add_rvalue_reference_t<value_type>
			>
			requires (!std::is_void_v<value_type>)
		{
			return *std::forward<decltype(instance)> (instance);
		}

		[[nodiscard]]
		static constexpr auto getValueOr(
			flex::forward_of<type> auto&& instance,
			flex::forward_of<value_type> auto&& defaultValue
		) noexcept -> value_type requires (!std::is_void_v<value_type>) {
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.value_or(std::forward<decltype(defaultValue)> (defaultValue));
			}) {
				return std::forward<decltype(instance)> (instance)
					.value_or(std::forward<decltype(defaultValue)> (defaultValue));
			}
			else {
				return !std::forward<decltype(instance)>
					? std::forward<decltype(defaultValue)>
					: *std::forward<decltype(instance)> (instance);
			}
		}

		[[nodiscard]]
		static constexpr auto andThen(flex::forward_of<type> auto&& instance, auto&& func)
			noexcept(noexcept(std::forward<decltype(func)> (func)(*std::forward<decltype(instance)> (instance))))
			requires (!std::is_void_v<value_type>)
		{
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.and_then(std::forward<decltype(func)> (func));
			}) {
				return std::forward<decltype(instance)> (instance)
					.and_then(std::forward<decltype(func)> (func));
			}
			else {
				using Ret = decltype(std::forward<decltype(func)> (func)(
					*std::forward<decltype (instance)> (instance)
				));
				if (!!std::forward<decltype(instance)>)
					return std::forward<decltype(func)> (func)(*std::forward<decltype (instance)> (instance));
				else
					return std::remove_cvref_t<Ret> {};
			}
		}

		[[nodiscard]]
		static constexpr auto transform(flex::forward_of<type> auto&& instance, auto&& func)
			noexcept(noexcept(std::forward<decltype(func)> (func)(*std::forward<decltype(instance)> (instance))))
			requires (!std::is_void_v<value_type>)
		{
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.transform(std::forward<decltype(func)> (func));
			}) {
				return std::forward<decltype(instance)> (instance)
					.transform(std::forward<decltype(func)> (func));
			}
			else {
				using Ret = decltype(type{std::forward<decltype(func)> (func)(
					*std::forward<decltype (instance)> (instance)
				)});
				if (!!std::forward<decltype(instance)>)
					return type{std::forward<decltype(func)> (func)(*std::forward<decltype (instance)> (instance))};
				else
					return std::remove_cvref_t<Ret> {};
			}
		}

		[[nodiscard]]
		static constexpr auto or_else(flex::forward_of<type> auto&& instance, auto&& func)
			noexcept(noexcept(std::forward<decltype(func)> (func)()))
			requires (!std::is_void_v<value_type>)
		{
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.or_else(std::forward<decltype(func)> (func));
			}) {
				return std::forward<decltype(instance)> (instance)
					.or_else(std::forward<decltype(func)> (func));
			}
			else {
				return !std::forward<decltype(instance)> (instance)
					? std::forward<func> (func)()
					: *std::forward<decltype(instance)> (instance);
			}
		}
	};


	template <internals::autogen_error_type_with_error T>
	struct error_type_traits<T> {
		using type = T;
		using value_type = typename T::value_type;
		using error_type = typename T::error_type;

		[[nodiscard]]
		static constexpr auto hasValue(flex::forward_of<type> auto&& instance) noexcept -> bool {
			return !!std::forward<decltype(instance)> (instance);
		}
		[[nodiscard]]
		static constexpr auto getValue(flex::forward_of<type> auto&& instance) noexcept
			-> std::conditional_t<std::is_lvalue_reference_v<decltype(instance)>,
				std::add_lvalue_reference_t<value_type>, std::add_rvalue_reference_t<value_type>
			>
			requires (!std::is_void_v<value_type>)
		{
			return *std::forward<decltype(instance)> (instance);
		}
		[[nodiscard]]
		static constexpr auto getError(flex::forward_of<type> auto&& instance) noexcept
			-> std::conditional_t<std::is_lvalue_reference_v<decltype(instance)>,
				std::add_lvalue_reference_t<error_type>, std::add_rvalue_reference_t<error_type>
			>
		{
			return std::forward<decltype(instance)> (instance).error();
		}

		[[nodiscard]]
		static constexpr auto getValueOr(
			flex::forward_of<type> auto&& instance,
			flex::forward_of<value_type> auto&& defaultValue
		) noexcept -> value_type requires (!std::is_void_v<value_type>) {
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.value_or(std::forward<decltype(defaultValue)> (defaultValue));
			}) {
				return std::forward<decltype(instance)> (instance)
					.value_or(std::forward<decltype(defaultValue)> (defaultValue));
			}
			else {
				return !std::forward<decltype(instance)>
					? std::forward<decltype(defaultValue)>
					: *std::forward<decltype(instance)> (instance);
			}
		}

		[[nodiscard]]
		static constexpr auto getErrorOr(
			flex::forward_of<type> auto&& instance,
			flex::forward_of<error_type> auto&& defaultValue
		) noexcept -> value_type {
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.error_or(std::forward<decltype(defaultValue)> (defaultValue));
			}) {
				return std::forward<decltype(instance)> (instance)
					.error_or(std::forward<decltype(defaultValue)> (defaultValue));
			}
			else {
				return !std::forward<decltype(instance)>
					? std::forward<decltype(defaultValue)>
					: std::forward<decltype(instance)> (instance).error();
			}
		}

		[[nodiscard]]
		static constexpr auto andThen(flex::forward_of<type> auto&& instance, auto&& func)
			noexcept(noexcept(std::forward<decltype(func)> (func)(*std::forward<decltype(instance)> (instance))))
			requires (!std::is_void_v<value_type>)
		{
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.and_then(std::forward<decltype(func)> (func));
			}) {
				return std::forward<decltype(instance)> (instance)
					.and_then(std::forward<decltype(func)> (func));
			}
			else {
				using Ret = decltype(std::forward<decltype(func)> (func)(
					*std::forward<decltype (instance)> (instance)
				));
				if (!!std::forward<decltype(instance)>)
					return std::forward<decltype(func)> (func)(*std::forward<decltype (instance)> (instance));
				else
					return std::remove_cvref_t<Ret> {};
			}
		}

		[[nodiscard]]
		static constexpr auto transform(flex::forward_of<type> auto&& instance, auto&& func)
			noexcept(noexcept(std::forward<decltype(func)> (func)(*std::forward<decltype(instance)> (instance))))
			requires (!std::is_void_v<value_type>)
		{
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.transform(std::forward<decltype(func)> (func));
			}) {
				return std::forward<decltype(instance)> (instance)
					.transform(std::forward<decltype(func)> (func));
			}
			else {
				using Ret = decltype(type{std::forward<decltype(func)> (func)(
					*std::forward<decltype (instance)> (instance)
				)});
				if (!!std::forward<decltype(instance)>)
					return type{std::forward<decltype(func)> (func)(*std::forward<decltype (instance)> (instance))};
				else
					return std::remove_cvref_t<Ret> {};
			}
		}

		[[nodiscard]]
		static constexpr auto or_else(flex::forward_of<type> auto&& instance, auto&& func)
			noexcept(noexcept(std::forward<decltype(func)> (func)()))
			requires (!std::is_void_v<value_type>)
		{
			if constexpr (requires {std::forward<decltype(instance)> (instance)
				.or_else(std::forward<decltype(func)> (func));
			}) {
				return std::forward<decltype(instance)> (instance)
					.or_else(std::forward<decltype(func)> (func));
			}
			else {
				return !std::forward<decltype(instance)> (instance)
					? std::forward<func> (func)()
					: *std::forward<decltype(instance)> (instance);
			}
		}
	};

	static_assert(internals::autogen_error_type<std::optional<int>>);
	static_assert(!internals::autogen_error_type_with_error<std::optional<int>>);
	static_assert(error_type<std::optional<int>>);
}
