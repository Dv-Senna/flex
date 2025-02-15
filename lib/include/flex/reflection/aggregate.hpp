#pragma once

#include <cstddef>
#include <source_location>
#include <string_view>

#include "flex/reflection/autogen/aggregateMembersTuple.hpp"
#include "flex/reflection/aggregateMembersCount.hpp"


namespace flex::reflection {
	namespace __internals {
		template <typename T>
		extern const T fakeObject;

		template <auto ptr>
		consteval auto nameGiverFunction() noexcept -> std::string_view {
			return std::source_location::current().function_name();
		}

		template <typename T>
		struct PointerWrapper {
			T *ptr;
		};

		template <std::size_t N, typename S>
		consteval auto makePointer() noexcept {
			auto &member {std::get<N> (flex::reflection::makeAggregateMembersTuple(fakeObject<S>))};
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

	template <std::size_t N, typename S>
	requires (N < flex::reflection::getAggregateMembersCount<S> ())
	consteval auto getAggregateMemberName() noexcept -> std::string_view {
	#ifdef __clang__
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wundefined-var-template"
	#endif
		return __internals::nameStripper(__internals::nameGiverFunction<__internals::makePointer<N, S> ()> ());
	#ifdef __clang__
		#pragma clang diagnostic pop
	#endif
	};

} // namespace flex::reflection
