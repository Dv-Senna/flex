#pragma once

#include <cstddef>
#include <tuple>

#include "flex/reflection/aggregate.hpp"
#include "flex/reflection/aggregateMembersCount.hpp"
#include "flex/reflection/metadata.hpp"


namespace flex {
	template <typename T>
	constexpr auto is_autogen_reflection_v = std::is_aggregate_v<T> && !flex::reflection::custom<T>;

	template <typename T>
	concept autogen_reflection = is_autogen_reflection_v<T>;


	template <typename T>
	struct reflection_traits {
		using type = std::remove_cvref_t<T>;
		static constexpr bool IS_REFLECTABLE {false};
	};

	template <autogen_reflection T>
	struct reflection_traits<T> {
		using type = std::remove_cvref_t<T>;
		static constexpr bool IS_REFLECTABLE {true};
		static constexpr std::size_t MEMBERS_COUNT {flex::reflection::aggregate_members_count_v<type>};
		static constexpr std::tuple MEMBERS_NAMES {flex::reflection::aggregate_members_names_v<type>};
		using MembersTypes = flex::reflection::aggregate_members_t<type>;

		template <std::size_t N>
		static constexpr auto getMember(std::add_lvalue_reference_t<type> instance) noexcept
			-> std::add_lvalue_reference_t<std::tuple_element_t<N, MembersTypes>> {
			return std::get<N> (flex::reflection::makeAggregateMembersTuple(instance));
		}

		template <std::size_t N>
		static constexpr auto getMember(std::add_lvalue_reference_t<std::add_const_t<type>> instance) noexcept
			-> std::add_lvalue_reference_t<std::add_const_t<std::tuple_element_t<N, MembersTypes>>> {
			return std::get<N> (flex::reflection::makeAggregateMembersTuple(instance));
		}
	};

	template <flex::reflection::custom T>
	struct reflection_traits<T> {
		using type = std::remove_cvref_t<T>;
		static constexpr bool IS_REFLECTABLE {true};
		static constexpr std::size_t MEMBERS_COUNT {std::tuple_size_v<decltype(T::FlexMetadata::MEMBERS)>};
		static constexpr std::tuple MEMBERS_NAMES {flex::reflection::custom_members_names_v<type>};
		using MembersTypes = flex::reflection::custom_members_t<type>;

		template <std::size_t N>
		static constexpr auto getMember(std::add_lvalue_reference_t<type> instance) noexcept -> flex::reflection::custom_member_access_t<N, type, false> {
			return instance.*flex::reflection::custom_member_pointer_v<N, type>;
		}

		template <std::size_t N>
		static constexpr auto getMember(std::add_lvalue_reference_t<std::add_const_t<type>> instance) noexcept -> flex::reflection::custom_member_access_t<N, type, true> {
			return instance.*flex::reflection::custom_member_pointer_v<N, type>;
		}
	};



	template <typename T>
	constexpr auto is_reflectable_v = reflection_traits<T>::IS_REFLECTABLE;

	template <typename T>
	concept reflectable = flex::is_reflectable_v<T>;


	template <reflectable T>
	constexpr auto reflection_members_count_v = reflection_traits<T>::MEMBERS_COUNT;

	template <reflectable T>
	constexpr auto reflection_members_names_v = reflection_traits<T>::MEMBERS_NAMES;

	template <reflectable T>
	using reflection_members_t = typename reflection_traits<T>::MembersTypes;

} // namespace flex
