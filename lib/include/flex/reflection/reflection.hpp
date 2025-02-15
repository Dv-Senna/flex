#pragma once

#include <cstddef>
#include <tuple>

#include "flex/reflection/aggregate.hpp"
#include "flex/reflection/aggregateMembersCount.hpp"
#include "flex/typeTraits.hpp"


namespace flex {
	template <typename T>
	struct reflection_traits {
		using type = T;
		static constexpr bool IS_REFLECTABLE {false};
	};

	template <flex::aggregate T>
	struct reflection_traits<T> {
		using type = T;
		static constexpr bool IS_REFLECTABLE {true};
		static constexpr std::size_t MEMBERS_COUNT {flex::reflection::aggregate_members_count_v<T>};
		static constexpr std::tuple MEMBERS_NAMES {flex::reflection::aggregate_members_names_v<T>};
		using MembersTypes = typename flex::reflection::aggregate_members_t<T>;

		template <std::size_t N>
		static constexpr auto getMember(T &instance) noexcept -> std::add_lvalue_reference_t<std::tuple_element_t<N, MembersTypes>> {
			return std::get<N> (flex::reflection::makeAggregateMembersTuple(instance));
		}

		template <std::size_t N>
		static constexpr auto getMember(const T &instance) noexcept -> std::add_lvalue_reference_t<std::add_const_t<std::tuple_element_t<N, MembersTypes>>> {
			return std::get<N> (flex::reflection::makeAggregateMembersTuple(instance));
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
