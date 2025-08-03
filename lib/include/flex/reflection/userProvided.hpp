#pragma once

#include <tuple>
#include <type_traits>

#include "flex/core/typeTraits.hpp"


namespace flex::reflection {
	template <typename T>
	concept has_user_provided_metadata = requires {
		typename T::FlexMetadata;
	};

	namespace internals {
		template <
			flex::tuple T,
			std::size_t I = 0,
			std::size_t N = std::tuple_size<T>::value
		>
		struct is_user_provided_metadata_rename_valid : std::bool_constant<
			flex::tuple<std::tuple_element_t<I, T>>
			&& std::tuple_size<std::tuple_element_t<I, T>>::value == 2
			&& flex::string<typename std::tuple_element<0, std::tuple_element_t<I, T>>::type>
			&& std::is_member_object_pointer<typename std::tuple_element<1, std::tuple_element_t<I, T>>::type>::value
			&& is_user_provided_metadata_rename_valid<T, I + 1, N>::value
		> {};

		template <typename T, std::size_t N>
		struct is_user_provided_metadata_rename_valid<T, N, N> : std::true_type {};


		template <
			flex::tuple T,
			std::size_t I = 0,
			std::size_t N = std::tuple_size<T>::value
		>
		struct is_user_provided_metadata_remove_valid : std::bool_constant<
			std::is_member_object_pointer<std::tuple_element_t<I, T>>::value
			&& is_user_provided_metadata_remove_valid<T, I + 1, N>::value
		> {};

		template <typename T, std::size_t N>
		struct is_user_provided_metadata_remove_valid<T, N, N> : std::true_type {};


		template <typename T>
		struct is_getter;// : std::false_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_getter<T (S::*)() const> : std::true_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_getter<T (S::*)() const noexcept> : std::true_type {};

		template <typename T>
		concept getter = is_getter<typename std::remove_cvref<T>::type>::value;


		template <typename T>
		struct is_setter;// : std::false_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_setter<void (S::*)(T)> : std::true_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_setter<void (S::*)(T) noexcept> : std::true_type {};

		template <typename T>
		concept setter = is_setter<typename std::remove_cvref<T>::type>::value;


		template <typename Getter, typename Setter>
		struct is_getter_setter_pair;// : std::false_type {};

		template <getter Getter, setter Setter>
		struct is_getter_setter_pair<Getter, Setter> : std::bool_constant<
			std::invocable<
				flex::member_pointer_extractor_t<Setter>,
				typename std::invoke_result<flex::member_pointer_extractor_t<Getter>>::type
			>
		> {};

		template <setter Setter, getter Getter>
		struct is_getter_setter_pair<Setter, Getter> : is_getter_setter_pair<Getter, Setter> {};


		template <typename T, std::size_t N = std::tuple_size<T>::value>
		struct is_user_provided_metadata_new_member_field_valid : std::false_type {};

		template <typename T>
		struct is_user_provided_metadata_new_member_field_valid<T, 2> : std::bool_constant<
			flex::tuple<T>
			&& flex::string<typename std::tuple_element<0, T>::type>
			&& (
				getter<typename std::tuple_element<1, T>::type>
				|| setter<typename std::tuple_element<1, T>::type>
			)
		> {};

		template <typename T>
		struct is_user_provided_metadata_new_member_field_valid<T, 3> : std::bool_constant<
			flex::tuple<T>
			&& flex::string<typename std::tuple_element<0, T>::type>
			&& is_getter_setter_pair<
				typename std::tuple_element<1, T>::type,
				typename std::tuple_element<2, T>::type
			>::value
		> {};


		template <
			flex::tuple T,
			std::size_t I = 0,
			std::size_t N = std::tuple_size<T>::value
		>
		struct is_user_provided_metadata_new_member_valid : std::bool_constant<
			is_user_provided_metadata_new_member_field_valid<std::tuple_element_t<I, T>>::value
			&& is_user_provided_metadata_new_member_valid<T, I + 1, N>::value
		> {};

		template <typename T, std::size_t N>
		struct is_user_provided_metadata_new_member_valid<T, N, N> : std::true_type {};
	}


	template <typename T>
	concept user_provided_metadata_has_rename = requires {
		{T::rename} -> flex::tuple;
	}
		&& std::tuple_size<decltype(T::rename)>::value > 0
		&& internals::is_user_provided_metadata_rename_valid<decltype(T::rename)>::value;

	template <typename T>
	concept user_provided_metadata_has_remove = requires {
		{T::remove} -> flex::tuple;
	}
		&& std::tuple_size<decltype(T::remove)>::value > 0
		&& internals::is_user_provided_metadata_remove_valid<decltype(T::remove)>::value;

	template <typename T>
	concept user_provided_metadata_has_new_member = requires {
		{T::new_member} -> flex::tuple;
	}
		&& std::tuple_size<decltype(T::new_member)>::value > 0
		&& internals::is_user_provided_metadata_new_member_valid<decltype(T::new_member)>::value;
}
