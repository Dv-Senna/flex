#pragma once

#include <concepts>
#include <cstddef>

#include "flex/core/typeTraits.hpp"
#include "flex/macros/macros.hpp"


namespace flex::reflection::aggregate {
	template <flex::aggregate T>
	struct member_count {
		private:
			template <typename ...Args>
			static constexpr auto computeValue() noexcept -> std::size_t {
			#ifdef __cpp_impl_reflection
				#error C++26 reflection not implemented for now
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


	template <typename T>
	constexpr auto getMemberTie(T& instance) noexcept {
	#ifdef __cpp_impl_reflection
		#error C++26 reflection not implemented for now
	#elif defined(__cpp_structured_bindings) && __cpp_structured_bindings >= 202411L
		auto& [...members] {instance};
		return std::tie(members...);
	#else
		constexpr auto memberCount {member_count<T>::value};
		if constexpr (memberCount == 0)
			return std::tuple<> {};
	#define FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK_BODY(ctx, step_ctx, ...) e##__VA_ARGS__
	#define FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK(size) FLEX_MACROS_IOTA_FOR_BODY_WITH_SEP2( \
		FLEX_MACROS_INCREMENT(size), FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK_BODY, FLEX_MACROS_COMMA, \
			FLEX_MACROS_NULL)
	#define FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_BODY(ctx, step_ctx, ...) \
		if constexpr (memberCount == FLEX_MACROS_INCREMENT(__VA_ARGS__)) { \
			auto& [FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK(__VA_ARGS__)] {instance}; \
			return std::tie(FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK(__VA_ARGS__)); \
		}

		FLEX_MACROS_IOTA_FOR_BODY(16,
			FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_BODY,
			FLEX_MACROS_NULL
		)

	#undef FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_BODY
	#undef FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK
	#undef FLEX_REFLECTION_AGGREGATE_GET_MEMBER_TIE_PACK_BODY
	#endif
	};
}
