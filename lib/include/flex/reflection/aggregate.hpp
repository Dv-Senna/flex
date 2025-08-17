#pragma once

#include <concepts>
#include <cstddef>
#include <source_location>
#include <string_view>

#ifdef __cpp_impl_reflection
	#include <meta>
#endif

#include "flex/core/typeTraits.hpp"
#include "flex/macros/macros.hpp"


namespace flex::reflection::aggregate {
	template <flex::aggregate T>
	struct member_count {
		private:
			template <typename ...Args>
			static constexpr auto computeValue() noexcept -> std::size_t {
			#ifdef __cpp_impl_reflection
				constexpr auto ctx {std::meta::access_context::current()};
				return nonstatic_data_members_of(^^T, ctx).size();
			#else
				if constexpr (std::constructible_from<T, flex::AnyTypePlaceholder, Args...>)
					return computeValue<flex::AnyTypePlaceholder, Args...> ();
				else
					return sizeof...(Args);
			#endif
			}

		public:
			static constexpr auto value {computeValue<> ()};
			using value_type = decltype(value);
	};

	template <typename T>
	constexpr auto member_count_v = member_count<T>::value;


	template <flex::aggregate T>
	constexpr auto getMemberTie(T& instance) noexcept {
	#if defined(__cpp_structured_bindings) && __cpp_structured_bindings >= 202411L
		auto& [...members] {instance};
		return std::tie(members...);
	#else
		constexpr auto memberCount {member_count<T>::value};
		if constexpr (memberCount == 0)
			return std::tuple<> {};
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
	#define FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK_BODY(ctx, step_ctx, ...) e##__VA_ARGS__
	#define FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK(size) FLEX_MACROS_IOTA_FOR_BODY_WITH_SEP2( \
		FLEX_MACROS_INCREMENT(size), FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK_BODY, FLEX_MACROS_COMMA, \
			FLEX_MACROS_NULL)
	#define FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_BODY(ctx, step_ctx, ...) \
		if constexpr (memberCount == FLEX_MACROS_INCREMENT(__VA_ARGS__)) { \
			auto& [FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK(__VA_ARGS__)] {instance}; \
			return std::tie(FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK(__VA_ARGS__)); \
		}
// NOLINTEND(cppcoreguidelines-macro-usage)

		FLEX_MACROS_IOTA_FOR_BODY(FLEX_REFLECTION_MAX_MEMBERS_COUNT,
			FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_BODY,
			FLEX_MACROS_NULL
		)

	#undef FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_BODY
	#undef FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK
	#undef FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK_BODY
	#endif
	};


	namespace internals {
		template <typename T>
		extern T fakeObject;

		template <auto ptr>
		consteval auto getRawMemberName() noexcept -> std::string_view {
			return std::string_view{std::source_location::current().function_name()};
		}

		consteval auto stripRawMemberName(std::string_view name) noexcept -> std::string_view {
			using namespace std::string_view_literals;
		#if defined(__clang__)
			name = {name.begin() + name.find("fakeObject.") + "fakeObject."sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find("}]")};
		#elif defined(__GNUC__)
			name = {name.begin() + name.find("fakeObject") + "fakeObject"sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find(")};")};
			name = {name.begin() + name.rfind("::") + "::"sv.size(), name.end()};
		#elif defined(_MSC_VER)
			name = {name.begin() + name.find("->") + "->"sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find("}")};
		#endif
			return name;
		}

		template <typename T>
		struct PointerMemberWrapper {
			T* ptr;
		};

		template <std::size_t N, typename T>
		consteval auto makePointer() noexcept {
		#ifdef __clang__
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wundefined-var-template"
		#endif
			auto& member {std::get<N> (getMemberTie(fakeObject<T>))};
		#ifdef __clang__
			#pragma clang diagnostic pop
		#endif
			return PointerMemberWrapper<std::remove_reference_t<decltype(member)>> (&member);
		}
	}


	template <flex::aggregate T>
	using get_member_types_t = typename flex::remove_tuple_reference<
		decltype(getMemberTie(internals::fakeObject<T>))
	>::type;


	template <flex::aggregate T>
	consteval auto getMemberNames() noexcept {
		constexpr auto memberCount {member_count<T>::value};
		std::array<std::string_view, memberCount> results {};
	#ifdef __cpp_impl_reflection
		constexpr auto ctx {std::meta::access_context::current()};
		std::size_t i {0};
		template for (constexpr auto member : std::define_static_array(nonstatic_data_members_of(^^T, ctx))) {
			if constexpr (has_identifier(member))
				results[i++] = std::string_view{identifier_of(member)};
			else
				results[i++] = "<unnamed>";
		}
	#else
		auto loop {[&] <std::size_t I = 0> (auto& self) {
			if constexpr (I + 1 < memberCount)
				self.template operator() <I + 1> (self);
			results[I] = internals::stripRawMemberName(internals::getRawMemberName<internals::makePointer<I, T> ()> ());
		}};
		loop(loop);
	#endif
		return results;
	}
}
