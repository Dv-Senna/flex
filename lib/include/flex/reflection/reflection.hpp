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
		static constexpr bool HAS_REFLECTION {false};
	};

	template <flex::aggregate T>
	struct reflection_traits<T> {
		using type = T;
		static constexpr bool HAS_REFLECTION {true};
		static constexpr std::size_t MEMBERS_COUNT {flex::reflection::aggregate_members_count_v<T>};
		static constexpr std::tuple MEMBERS_NAMES {flex::reflection::aggregate_members_names_v<T>};
		using MembersTypes = typename flex::reflection::aggregate_members_t<T>;
	};


	template <typename T>
	concept reflectable = flex::reflection_traits<T>::HAS_REFLECTION;

} // namespace flex
