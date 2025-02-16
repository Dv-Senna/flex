#pragma once

#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "flex/reflection/aggregate.hpp"
#include "flex/typeTraits.hpp"


namespace flex::reflection {
	template <typename T, typename = void>
	struct is_custom : std::false_type {};

	template <typename T>
	struct is_custom<T,
		std::enable_if_t<
			flex::true_v<typename T::FlexMetadata>
			&& flex::is_tuple_v<std::remove_cvref_t<decltype(T::FlexMetadata::MEMBERS)>>
		, void>
	> : std::true_type {};

	template <typename T>
	constexpr auto is_custom_v = is_custom<T>::value;

	template <typename T>
	concept custom = is_custom_v<T>;


	template <custom T>
	constexpr std::size_t custom_members_count_v = std::tuple_size_v<std::remove_cvref_t<decltype(T::FlexMetadata::MEMBERS)>>;

	template <std::size_t N, custom T>
	using custom_member_metadata_field_t = std::remove_cvref_t<std::tuple_element_t<N, std::remove_cvref_t<decltype(T::FlexMetadata::MEMBERS)>>>;

	template <std::size_t N, custom T>
	constexpr auto custom_member_metadata_field_v = std::get<N> (T::FlexMetadata::MEMBERS);



	namespace __internals {
		consteval auto stripCustomName(std::string_view name) noexcept -> std::string_view {
			using namespace std::string_view_literals;
		#ifdef __clang__
			name = {name.begin() + name.find("ptr = ") + "ptr = "sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find("]")};
			name = {name.begin() + name.rfind("::") + "::"sv.size(), name.end()};
		#elifdef __GNUC__
			name = {name.begin() + name.find("with auto ptr = ") + "with auto ptr = "sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find(";")};
			name = {name.begin() + name.rfind("::") + "::"sv.size(), name.end()};
		#elifdef _MSC_VER
			name = {name.begin() + name.find("getFuncName<") + "getFuncName<"sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find(">(void)")};
			name = {name.begin() + name.rfind("::") + "::"sv.size(), name.end()};
		#endif
			return name;
		};


		template <flex::tuple T, std::size_t N = 0, typename = void>
		requires (N < std::tuple_size_v<T>)
		struct name_index_from_tuple : name_index_from_tuple<T, N+1> {};

		template <flex::tuple T, std::size_t N>
		struct name_index_from_tuple<T, N,
			std::enable_if_t<
				flex::is_string_v<std::tuple_element_t<N, T>>
			, void>
		> : std::integral_constant<std::size_t, N> {};

		template <flex::tuple T>
		constexpr auto name_index_from_tuple_v = name_index_from_tuple<T>::value;

		template <std::size_t N, custom T>
		constexpr auto name_index_from_field_v = name_index_from_tuple_v<custom_member_metadata_field_t<N, T>>;


		template <std::size_t N, custom T>
		constexpr auto member_index_from_field_v = 1 - name_index_from_field_v<N, T>;

	} // namespace __internals


	template <std::size_t N, custom T, typename = void>
	requires (N < custom_members_count_v<T>)
	struct custom_member_name {
		using value_type = std::string_view;
		static constexpr value_type value {__internals::stripCustomName(__internals::nameGiverFunction<custom_member_metadata_field_v<N, T>> ())};
	};

	template <std::size_t N, custom T>
	struct custom_member_name<N, T,
		std::enable_if_t<
			flex::is_tuple_v<custom_member_metadata_field_t<N, T>>
		, void>
	> {
		using value_type = std::string_view;
		static constexpr value_type value {std::get<__internals::name_index_from_field_v<N, T>> (custom_member_metadata_field_v<N, T>)};
	};

	template <std::size_t N, custom T>
	constexpr auto custom_member_name_v = custom_member_name<N, T>::value;


	template <custom T, std::size_t N = 0, std::size_t COUNT = custom_members_count_v<T>>
	struct custom_members_names {
		static constexpr std::tuple value {std::tuple_cat(
			std::make_tuple(custom_member_name_v<N, T>),
			custom_members_names<T, N+1>::value
		)};
		using value_type = decltype(value);
	};

	template <custom T, std::size_t COUNT>
	struct custom_members_names<T, COUNT, COUNT> {
		using value_type = std::tuple<>;
		static constexpr value_type value {};
	};

	template <custom T>
	constexpr auto custom_members_names_v {custom_members_names<T>::value};


	template <std::size_t N, custom T, typename = void>
	struct custom_member {
		using type = flex::member_pointer_extractor_t<custom_member_metadata_field_t<N, T>>;
	};

	template <std::size_t N, custom T>
	struct custom_member<N, T,
		std::enable_if_t<
			flex::is_tuple_v<custom_member_metadata_field_t<N, T>>
		, void>
	> {
		using type = flex::member_pointer_extractor_t<std::remove_cvref_t<std::tuple_element_t<__internals::member_index_from_field_v<N, T>, custom_member_metadata_field_t<N, T>>>>;
	};

	template <std::size_t N, custom T>
	using custom_member_t = typename custom_member<N, T>::type;


	template <custom T, std::size_t N = 0, std::size_t COUNT = custom_members_count_v<T>>
	struct custom_members {
		using type = decltype(std::tuple_cat(
			std::make_tuple(std::declval<custom_member_t<N, T>> ()),
			std::declval<typename custom_members<T, N+1>::type> ()
		));
	};

	template <custom T, std::size_t COUNT>
	struct custom_members<T, COUNT, COUNT> {
		using type = std::tuple<>;
	};

	template <custom T>
	using custom_members_t = typename custom_members<T>::type;


	template <std::size_t N, custom T, typename = void>
	struct custom_member_pointer {
		static constexpr auto value {custom_member_metadata_field_v<N, T>};
		using value_type = decltype(value);
	};

	template <std::size_t N, custom T>
	struct custom_member_pointer<N, T,
		std::enable_if_t<
			flex::is_tuple_v<custom_member_metadata_field_t<N, T>>
		, void>
	> {
		static constexpr auto value {std::get<__internals::member_index_from_field_v<N, T>> (custom_member_metadata_field_v<N, T>)};
		using value_type = decltype(value);
	};

	template <std::size_t N, custom T>
	constexpr auto custom_member_pointer_v = custom_member_pointer<N, T>::value;


	template <std::size_t N, custom T, bool CONSTANT>
	struct custom_member_access {
		using type = std::add_lvalue_reference_t<std::tuple_element_t<N, custom_members_t<T>>>;
	};

	template <std::size_t N, custom T>
	struct custom_member_access<N, T, true> {
		using type = std::add_lvalue_reference_t<std::add_const_t<std::tuple_element_t<N, custom_members_t<T>>>>;
	};

	template <std::size_t N, custom T, bool CONSTANT>
	using custom_member_access_t = typename custom_member_access<N, T, CONSTANT>::type;

} // namespace flex::reflection
