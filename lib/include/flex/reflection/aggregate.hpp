#pragma once

#include <array>
#include <cstddef>
#include <source_location>
#include <string_view>

#include "flex/reflection/autogen/aggregateMembersTuple.hpp"
#include "flex/reflection/aggregateMembersCount.hpp"
#include "flex/typeTraits.hpp"


namespace flex::reflection {
	namespace __internals {
		template <flex::aggregate T>
		extern T fakeObject;

		template <auto ptr>
		consteval auto nameGiverFunction() noexcept -> std::string_view {
			return std::source_location::current().function_name();
		}

		template <typename T>
		struct PointerWrapper {
			T *ptr;
		};

		template <std::size_t N, flex::aggregate T>
		consteval auto makePointer() noexcept {
			auto &member {std::get<N> (flex::reflection::makeAggregateMembersTuple(fakeObject<T>))};
			return PointerWrapper<std::remove_reference_t<decltype(member)>> {&member};
		}

		consteval auto nameStripper(std::string_view name) noexcept -> std::string_view {
			using namespace std::string_view_literals;
		#ifdef __clang__
			name = {name.begin() + name.find("fakeObject.") + "fakeObject."sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find("}]")};
		#elifdef __GNUC__
			name = {name.begin() + name.find("fakeObject") + "fakeObject"sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find(")};")};
			name = {name.begin() + name.rfind("::") + "::"sv.size(), name.end()};
		#elifdef _MSC_VER
			name = {name.begin() + name.find("->") + "->"sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find("}")};
		#endif
			return name;
		}

	} // namespace __internals

	template <std::size_t N, flex::aggregate T>
	requires (N < flex::reflection::aggregate_members_count_v<T>)
	consteval auto getAggregateMemberName() noexcept -> std::string_view {
	#ifdef __clang__
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wundefined-var-template"
	#endif
		return __internals::nameStripper(__internals::nameGiverFunction<__internals::makePointer<N, T> ()> ());
	#ifdef __clang__
		#pragma clang diagnostic pop
	#endif
	};


	template <flex::aggregate T, std::size_t N = 0, std::size_t COUNT = flex::reflection::aggregate_members_count_v<T>>
	struct aggregate_members_names {
		static constexpr std::tuple value {std::tuple_cat(
			std::make_tuple(getAggregateMemberName<N, T> ()),
			aggregate_members_names<T, N+1, COUNT>::value
		)};
		using value_type = decltype(value);
	};

	template <flex::aggregate T, std::size_t COUNT>
	struct aggregate_members_names<T, COUNT, COUNT> {
		using value_type = std::tuple<>;
		static constexpr std::tuple<> value {};
	};

	template <flex::aggregate T>
	constexpr auto aggregate_members_names_v = aggregate_members_names<T>::value;


	template <flex::aggregate T>
	struct aggregate_members {
		using type = flex::remove_tuple_reference_t<decltype(flex::reflection::makeAggregateMembersTuple(flex::reflection::__internals::fakeObject<T>))>;
	};

	template <flex::aggregate T>
	using aggregate_members_t = typename aggregate_members<T>::type;

} // namespace flex::reflection
