#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

#include "flex/core/typeTraits.hpp"


namespace flex::pipes {
	template <typename T>
	struct outcome_traits;

	template <typename Traits>
	concept valid_outcome_traits = requires(typename Traits::type& lv, typename Traits::type&& rv) {
		typename Traits::type;
		typename Traits::value_type;
		typename Traits::error_type;

		Traits::value(lv);
		Traits::value(std::move(rv));
		noexcept(Traits::value(lv));
		noexcept(Traits::value(std::move(rv)));

		{Traits::has_value(lv)} -> std::same_as<bool>;
		{Traits::has_value(std::move(rv))} -> std::same_as<bool>;
		noexcept(Traits::has_value(lv));
		noexcept(Traits::has_value(std::move(rv)));
	};

	template <typename T>
	concept outcome = valid_outcome_traits<outcome_traits<std::remove_cvref_t<T>>>;

	template <typename T>
	concept autogen_outcome = requires(T& lv, T&& rv) {
		*lv;
		*rv;
		noexcept(*lv);
		noexcept(*rv);

		!lv;
		!rv;
		noexcept(!lv);
		noexcept(!rv);
	};

	template <typename T>
	concept autogen_error_outcome = autogen_outcome<T> && requires(T& lv, T&& rv) {
		lv.error();
		rv.error();
		noexcept(lv.error());
		noexcept(rv.error());
	};

	namespace internals {
		template <typename T>
		struct autogen_outcome_dispatch_value_type final : flex::type_constant<
			std::remove_reference_t<decltype(*std::declval<T> ())>
		> {};
		template <typename T>
		requires (requires{typename T::value_type;})
		struct autogen_outcome_dispatch_value_type<T> final : flex::type_constant<typename T::value_type> {};

		template <typename T>
		struct autogen_outcome_dispatch_error_type final : flex::type_constant<void> {};
		template <typename T>
		requires (requires{typename T::error_type;})
		struct autogen_outcome_dispatch_error_type<T> final : flex::type_constant<typename T::error_type> {};
		template <typename T>
		requires (autogen_error_outcome<T> && !requires{typename T::error_type;})
		struct autogen_outcome_dispatch_error_type<T> final : flex::type_constant<
			std::remove_reference_t<decltype(std::declval<T> ().error())>
		> {};
	}

	template <autogen_outcome T>
	struct outcome_traits<T> final {
		using type = T;
		using value_type = typename internals::autogen_outcome_dispatch_value_type<T>::type;
		using error_type = typename internals::autogen_outcome_dispatch_error_type<T>::type;

		static_assert(!std::is_reference_v<type>);
		static_assert(!std::is_reference_v<value_type>);
		static_assert(!std::is_reference_v<error_type>);

		static constexpr auto has_value(flex::forward_of<type> auto&& outcome) noexcept -> bool {
			return !!std::forward<decltype(outcome)> (outcome);
		}
		static constexpr auto value(flex::forward_of<type> auto&& outcome)
			noexcept
			-> decltype(*std::forward<decltype(outcome)> (outcome))
		{
			assert(outcome_traits::has_value(std::forward<decltype(outcome)> (outcome)));
			return *std::forward<decltype(outcome)> (outcome);
		}
		static constexpr auto error(flex::forward_of<type> auto&& outcome)
			noexcept
			-> decltype(std::forward<decltype(outcome)> (outcome).error())
			requires (!std::is_void_v<error_type>)
		{
			assert(!outcome_traits::has_value(std::forward<decltype(outcome)> (outcome)));
			return std::forward<decltype(outcome)> (outcome).error();
		}

		static constexpr auto value_or(flex::forward_of<type> auto&& outcome, flex::forward_of<value_type> auto&& value)
			noexcept
			-> value_type
			requires (flex::nothrow_forward_constructible<decltype(value)>)
		{
			if (outcome_traits::has_value(std::forward<decltype(outcome)> (outcome)))
				return outcome_traits::value(std::forward<decltype(outcome)> (outcome));
			return std::forward<decltype(value)> (value);
		}
		static constexpr auto error_or(flex::forward_of<type> auto&& outcome, flex::forward_of<error_type> auto&& error)
			noexcept
			-> error_type
			requires (flex::nothrow_forward_constructible<decltype(error)> && !std::is_void_v<error_type>)
		{
			if (!outcome_traits::has_value(std::forward<decltype(outcome)> (outcome)))
				return outcome_traits::error(std::forward<decltype(outcome)> (outcome));
			return std::forward<decltype(error)> (error);
		}
	};
}
