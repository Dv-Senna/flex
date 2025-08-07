#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <format>
#include <ranges>
#include <type_traits>

#include "flex/core/config.hpp"
#include "flex/core/stringifier.hpp"
#include "flex/core/typeTraits.hpp"
#include "flex/reflection/aggregate.hpp"
#include "flex/reflection/userProvided.hpp"


namespace flex::reflection {
	/**
	 * 
	 * */
	template <typename T>
	struct reflection_traits;

	namespace internals {
		template <typename T>
		concept pure_aggregate = flex::aggregate<T> && !flex::reflection::userProvided::has_metadata<T>;
	}

	template <internals::pure_aggregate T>
	struct reflection_traits<T> {
		using type = T;
		static constexpr auto name {flex::reflection::aggregate::getTypeName<T> ()};
		static constexpr auto member_count {flex::reflection::aggregate::member_count<T>::value};
		static constexpr auto member_names {flex::reflection::aggregate::getMemberNames<T> ()};
		using member_types = flex::reflection::aggregate::get_member_types_t<T>;

		template <std::unsigned_integral auto I>
		requires (I < member_count)
		static constexpr auto getMember(flex::variant_of<T> auto& instance) noexcept -> auto& {
			return std::get<I> (flex::reflection::aggregate::getMemberTie(instance));
		}
	};

	template <flex::reflection::userProvided::has_valid_metadata T>
	struct reflection_traits<T> {
		using type = T;
//		static constexpr auto name {};
		static constexpr auto member_names {flex::reflection::userProvided::getMemberNames<T> ()};
		static constexpr auto member_count {member_names.size()};

		template <std::unsigned_integral auto I>
		requires (I < member_count)
		static constexpr auto getMember(flex::variant_of<T> auto& instance) noexcept -> auto {
			return flex::reflection::userProvided::getMember<T, I> (instance);
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
		{Traits::name} -> std::convertible_to<std::string_view>;
		{Traits::member_count} -> internals::unsigned_integral;
		{Traits::member_names} -> internals::array_of_type<std::string_view>;
		typename Traits::member_types;
		Traits::template getMember<std::size_t{0}> (instance);
	}
		&& flex::tuple<typename Traits::member_types>
		&& internals::get_array_size_v<decltype(Traits::member_names)> == Traits::member_count
		&& std::tuple_size<typename Traits::member_types>::value == Traits::member_count;

	template <typename T>
	concept reflectable = complete_reflection_traits<reflection_traits<std::remove_reference_t<T>>>;


	namespace internals {
		template <typename T, typename Func,
			std::unsigned_integral auto I = decltype(reflection_traits<T>::member_count) {0},
			std::unsigned_integral auto N = reflection_traits<T>::member_count
		>
		struct is_foreach_member_func_noexcept : std::bool_constant<
			noexcept(std::declval<Func> ()(reflection_traits<T>::template getMember<I> (
				std::declval<std::add_lvalue_reference_t<T>> ()
			)))
			&& is_foreach_member_func_noexcept<T, Func, I + 1, N>::value
		> {};

		template <typename T, typename Func, std::unsigned_integral auto N>
		struct is_foreach_member_func_noexcept<T, Func, N, N> : std::true_type {};


		template <typename T, typename Func,
			std::unsigned_integral auto I = decltype(reflection_traits<T>::member_count) {0},
			std::unsigned_integral auto N = reflection_traits<T>::member_count
		>
		struct is_foreach_named_member_func_noexcept : std::bool_constant<
			noexcept(std::declval<Func> ()(reflection_traits<T>::template getMember<I> (
				std::declval<std::add_lvalue_reference_t<T>> ()
			), std::declval<std::string_view> ()))
			&& is_foreach_named_member_func_noexcept<T, Func, I + 1, N>::value
		> {};

		template <typename T, typename Func, std::unsigned_integral auto N>
		struct is_foreach_named_member_func_noexcept<T, Func, N, N> : std::true_type {};
	}

	constexpr auto foreachMember(reflectable auto&& instance, auto&& func) noexcept(
		internals::is_foreach_member_func_noexcept<std::remove_reference_t<decltype(instance)>, decltype(func)>::value
	) -> void {
		using T = std::remove_reference_t<decltype(instance)>;
		using CountT = decltype(reflection_traits<T>::member_count);
		auto loop {[&, instance = std::forward<decltype(instance)> (instance)]
			<std::unsigned_integral auto I = CountT{0}> (auto& loop) mutable
		{
			func(reflection_traits<T>::template getMember<I> (instance));
			if constexpr (I + 1 < reflection_traits<T>::member_count)
				loop.template operator() <I + 1> (loop);
		}};
		loop(loop);
	}

	constexpr auto foreachNamedMember(reflectable auto&& instance, auto&& func) noexcept(
		internals::is_foreach_named_member_func_noexcept<
			std::remove_reference_t<decltype(instance)>,
			decltype(func)
		>::value
	) -> void {
		using T = std::remove_reference_t<decltype(instance)>;
		using CountT = decltype(reflection_traits<T>::member_count);
		auto loop {[&, instance = std::forward<decltype(instance)> (instance)]
			<std::unsigned_integral auto I = CountT{0}> (auto& loop) mutable
		{
			func(
				reflection_traits<T>::template getMember<I> (instance),
				reflection_traits<T>::member_names[I]
			);
			if constexpr (I + 1 < reflection_traits<T>::member_count)
				loop.template operator() <I + 1> (loop);
		}};
		loop(loop);
	}


	namespace internals {
	#if __cplusplus >= FLEX_CPP_23
		template <typename T>
		concept formattable = std::formattable<T, char>;
	#else
		template <typename T>
		concept formattable = requires(T& value, std::format_context ctx) {
			std::formatter<std::remove_cvref_t<T>> ().format(value, ctx);
		};
	#endif
	}

	struct StringifyStyle {
		bool prettify {false};
		std::uint32_t indentSize {4};
		std::uint32_t currentIndent {0};
	};
}


namespace flex {
	template <flex::reflection::reflectable T>
	struct Stringifier<T> {
		constexpr auto operator() (
			flex::variant_of<T> auto&& value,
			const flex::reflection::StringifyStyle& style = {}
		) const noexcept -> std::string {
			std::string_view prefix {};
			std::string indent {};
			std::string result {"{"};
			if (style.prettify) {
				prefix = "\n";
				indent = std::string(style.currentIndent + style.indentSize, ' ');
			}

			auto processStringifyStyle = [](flex::reflection::StringifyStyle style) {
				if (!style.prettify)
					return style;
				style.currentIndent += style.indentSize;
				return style;
			};

			flex::reflection::foreachNamedMember(std::forward<decltype(value)> (value), [
				&style, &prefix, &result, &processStringifyStyle, &indent
			](
				auto& member, std::string_view name
			) {
				using Member = std::remove_reference_t<decltype(member)>;

				std::string memberValueAsString {};
				if constexpr (flex::reflection::internals::formattable<Member>)
					memberValueAsString = std::format("{}", member);
				else if constexpr (flex::failable_stringifyable<Member>) {
					using ErrorTraits = flex::error_type_traits<decltype(flex::toString(member))>;
					using ValueType = typename ErrorTraits::ValueType;
					if constexpr (flex::stringifyable_with<Member, flex::reflection::StringifyStyle>) {
						memberValueAsString = ErrorTraits::getValueOr(
							flex::toString(member, processStringifyStyle(style)), ValueType{"<to-string-error>"}
						);
					}
					else {
						memberValueAsString = ErrorTraits::getValueOr(
							flex::toString(member), ValueType{"<to-string-error>"}
						);
					}
				}
				else {
					if constexpr (flex::stringifyable_with<Member, flex::reflection::StringifyStyle>)
						memberValueAsString = flex::toString(member, processStringifyStyle(style));
					else
						memberValueAsString = flex::toString(member);
				}

				result += std::format("{}{}{}={}", prefix, indent, name, memberValueAsString);
				if (style.prettify)
					prefix = ",\n";
				else
					prefix = ",";
			});
			if (style.prettify) {
				result.push_back('\n');
				result += std::string(style.currentIndent, ' ');
			}
			result.push_back('}');
			return result;
		}
	};
}
