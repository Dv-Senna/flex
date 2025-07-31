#pragma once

#include <array>
#include <type_traits>

#include "flex/core/typeTraits.hpp"
#include "flex/reflection/aggregate.hpp"


namespace flex::reflection {
	/**
	 * 
	 * */
	template <typename T>
	struct reflection_traits;

	template <flex::aggregate T>
	struct reflection_traits<T> {
		using type = T;
		static constexpr auto member_count {flex::reflection::aggregate::member_count<T>::value};
		static constexpr auto member_names {flex::reflection::aggregate::getMemberNames<T> ()};
		using member_types = flex::reflection::aggregate::get_member_types_t<T>;

		template <std::unsigned_integral auto I>
		requires (I < member_count)
		static constexpr auto getMember(flex::variant_of<T> auto& instance) noexcept -> auto& {
			return std::get<I> (flex::reflection::aggregate::getMemberTie(instance));
		}
	};


	namespace internals {
		template <typename T, typename U>
		struct is_array_of_type : std::false_type {};

		template <typename U, auto N>
		struct is_array_of_type<std::array<U, N>, U> : std::true_type {};

		template <typename T, typename U>
		concept array_of_type = is_array_of_type<std::remove_cvref_t<T>, U>::value;


		template <typename T>
		struct get_array_size;

		template <typename T, auto N>
		struct get_array_size<std::array<T, N>> : std::integral_constant<decltype(N), N> {};

		template <typename T>
		constexpr auto get_array_size_v = get_array_size<std::remove_cvref_t<T>>::value;


		template <typename T>
		concept unsigned_integral = std::unsigned_integral<std::remove_cvref_t<T>>;
	}

	template <typename Traits>
	concept complete_reflection_traits = requires(typename Traits::type instance) {
		typename Traits::type;
		{Traits::member_count} -> internals::unsigned_integral;
		{Traits::member_names} -> internals::array_of_type<std::string_view>;
		typename Traits::member_types;
		Traits::template getMember<std::size_t{0}> (instance);
	}
		&& flex::tuple<typename Traits::member_types>
		&& internals::get_array_size_v<decltype(Traits::member_names)> == Traits::member_count
		&& std::tuple_size<typename Traits::member_types>::value == Traits::member_count;

	template <typename T>
	concept reflectable = complete_reflection_traits<reflection_traits<T>>;
}
