#pragma once

#include <array>
#include <concepts>
#include <format>
#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>

#include "flex/core/comptime.hpp"
#include "flex/core/stringifier.hpp"
#include "flex/core/typeTraits.hpp"
#include "flex/reflection/aggregate.hpp"
#include "flex/reflection/userProvided.hpp"

struct Person;

namespace flex::reflection {
	/**
	 * 
	 * */
	template <typename T>
	struct reflection_traits;

	namespace internals {
		template <typename T>
		concept pure_class_aggregate = flex::class_aggregate<T> && !flex::reflection::userProvided::has_metadata<T>;

		template <typename T>
		requires (std::is_class<T>::value)
		consteval auto getTypeName() noexcept -> std::string_view {
		#ifdef __cpp_impl_reflection
			if constexpr (has_identifier(^^T))
				return std::string_view{identifier_of(^^T)};
			else
				return std::string_view{display_string_of(^^T)};
		#else
			using namespace std::string_view_literals;
			std::string_view name {std::source_location::current().function_name()};
			#if defined(__clang__)
				name = name.substr(name.find("T = ") + "T = "sv.size());
				name = name.substr(0, name.find_first_of("]"));
			#elif defined(__GNUC__)
				name = name.substr(name.find("T = ") + "T = "sv.size());
				name = name.substr(0, name.find_first_of(";"));
			#elif defined(_MSC_VER)
				name = name.substr(name.find("getTypeName<") + "getTypeName<"sv.size());
				name = name.substr(name.find(" ") + " "sv.size());
				name = name.substr(0, name.find_last_of(">"));
			#endif
			return name;
		#endif
		}
	}

	template <internals::pure_class_aggregate T>
	struct reflection_traits<T> {
		using type = T;
		static constexpr auto name {internals::getTypeName<T> ()};
		static constexpr auto member_count {flex::reflection::aggregate::member_count<T>::value};
		static constexpr auto member_names {flex::reflection::aggregate::getMemberNames<T> ()};
		using member_types = flex::reflection::aggregate::get_member_types_t<T>;

		template <std::unsigned_integral auto I>
		requires (I < member_count)
		static constexpr auto getMember(flex::forward_of<T> auto& instance) noexcept -> auto& {
			return std::get<I> (flex::reflection::aggregate::getMemberTie(instance));
		}
	};

	template <flex::reflection::userProvided::has_valid_metadata T>
	struct reflection_traits<T> {
		using type = T;
		static constexpr auto name {internals::getTypeName<T> ()};
		static constexpr auto member_names {flex::reflection::userProvided::getMemberNames<T> ()};
		static constexpr auto member_count {member_names.size()};
		using member_types = flex::reflection::userProvided::get_member_types_t<T>;

		template <std::unsigned_integral auto I>
		requires (I < member_count)
		static constexpr auto getMember(flex::forward_of<T> auto& instance) noexcept -> auto {
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
		template <typename T>
		struct is_user_defined_member_wrapper : std::false_type {};

		template <typename S, auto getter, auto setter>
		struct is_user_defined_member_wrapper<
			userProvided::internals::MemberWrapper<S, getter, setter>
		> : std::true_type {};


		template <typename T>
		constexpr auto unwrapMember(T&& value) noexcept -> auto& {
			if constexpr (flex::is_specialization_of<T, std::reference_wrapper>::value)
				return std::forward<T> (value).get();
			else if constexpr (is_user_defined_member_wrapper<T>::value)
				return *std::forward<T> (value);
			else
				return std::forward<T> (value);
		}


		template <typename T, typename Func, std::unsigned_integral auto I, typename = void>
		struct is_member_func_noexcept : std::bool_constant<
			noexcept(std::declval<Func> ()(unwrapMember(
				reflection_traits<T>::template getMember<I> (
					std::declval<std::add_lvalue_reference_t<T>> ()
				)
			)))
		> {};

		template <typename T, typename Func, auto I>
		struct is_member_func_noexcept<T, Func, I,
			typename std::enable_if<flex::is_specialization_of<
				typename std::tuple_element<I, typename reflection_traits<T>::member_types>::type, WriteOnly
			>::value>::type
		> : std::true_type {};


		template <typename T, typename Func,
			std::unsigned_integral auto I = decltype(reflection_traits<T>::member_count) {0},
			std::unsigned_integral auto N = reflection_traits<T>::member_count
		>
		struct is_foreach_member_func_noexcept : std::bool_constant<
			is_member_func_noexcept<T, Func, I>::value
			&& is_foreach_member_func_noexcept<T, Func, I + 1, N>::value
		> {};

		template <typename T, typename Func, std::unsigned_integral auto N>
		struct is_foreach_member_func_noexcept<T, Func, N, N> : std::true_type {};


		template <typename First, typename Func, std::unsigned_integral auto I, typename = void, typename ...T>
		struct is_zip_member_func_noexcept : std::bool_constant<
			noexcept(std::declval<Func> ()(unwrapMember(
				reflection_traits<T>::template getMember<I> (
					std::declval<std::add_lvalue_reference_t<T>> ()
				)
			)...))
		> {};

		template <typename First, typename Func, auto I, typename ...T>
		struct is_zip_member_func_noexcept<First, Func, I,
			typename std::enable_if<flex::is_specialization_of<
				typename std::tuple_element<I, typename reflection_traits<First>::member_types>::type, WriteOnly
			>::value>::type,
		T...> : std::true_type {};


		template <typename First, typename Func,
			std::unsigned_integral auto I = decltype(reflection_traits<First>::member_count) {0},
			std::unsigned_integral auto N = reflection_traits<First>::member_count,
			typename ...T
		>
		struct is_foreach_zip_member_func_noexcept : std::bool_constant<
			is_zip_member_func_noexcept<First, Func, I, void, T...>::value
			&& is_foreach_zip_member_func_noexcept<First, Func, I + 1, N, T...>::value
		> {};

		template <typename First, typename Func, std::unsigned_integral auto N, typename ...T>
		struct is_foreach_zip_member_func_noexcept<First, Func, N, N, T...> : std::true_type {};

		template <typename Func, typename ...T>
		constexpr auto is_foreach_zip_member_func_noexcept_v = is_foreach_zip_member_func_noexcept<
			typename std::tuple_element<0, std::tuple<T...>>::type,
			Func,
			decltype(reflection_traits<typename std::tuple_element<0, std::tuple<T...>>::type>::member_count) {0},
			reflection_traits<typename std::tuple_element<0, std::tuple<T...>>::type>::member_count,
			T...
		>::value;


		template <typename T, typename Func, std::unsigned_integral auto I, typename = void>
		struct is_named_member_func_noexcept : std::bool_constant<
			noexcept(std::declval<Func> ()(unwrapMember(
				reflection_traits<T>::template getMember<I> (
					std::declval<std::add_lvalue_reference_t<T>> ()
				)
			), std::declval<std::string_view> ()))
		> {};


		template <typename T, typename Func, auto I>
		struct is_named_member_func_noexcept<T, Func, I,
			typename std::enable_if<flex::is_specialization_of<
				typename std::tuple_element<I, typename reflection_traits<T>::member_types>::type, WriteOnly
			>::value>::type
		> : std::true_type {};

		template <typename T, typename Func,
			std::unsigned_integral auto I = decltype(reflection_traits<T>::member_count) {0},
			std::unsigned_integral auto N = reflection_traits<T>::member_count
		>
		struct is_foreach_named_member_func_noexcept : std::bool_constant<
			is_named_member_func_noexcept<T, Func, I>::value
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
			using Member = typename std::tuple_element<I, typename reflection_traits<T>::member_types>::type;
			if constexpr (!flex::is_specialization_of<Member, WriteOnly>::value) {
				using FuncRet = typename std::invoke_result<
					decltype(func),
					std::add_lvalue_reference_t<std::tuple_element_t<I, typename reflection_traits<T>::member_types>>
				>::type;
				if constexpr (std::is_void<FuncRet>::value)
					func(internals::unwrapMember(reflection_traits<T>::template getMember<I> (instance)));
				else if constexpr (std::same_as<FuncRet, bool>) {
					if (!func(internals::unwrapMember(reflection_traits<T>::template getMember<I> (instance))))
						return;
				}
				else
					flex::comptimeErrorWithTypes<FuncRet> ("Can't have callback with the given return type");
			}
			if constexpr (I + 1 < reflection_traits<T>::member_count)
				loop.template operator() <I + 1> (loop);
		}};
		loop(loop);
	}

	constexpr auto zipForeachMember(auto&& func, reflectable auto&&... instance) noexcept(
		internals::is_foreach_zip_member_func_noexcept_v<
			decltype(func),
			std::remove_reference_t<decltype(instance)>...
		>
	) -> void
		requires (sizeof...(instance) > 0 && (std::same_as<
			std::remove_cvref_t<std::tuple_element_t<0, std::tuple<decltype(instance)...>>>,
			std::remove_cvref_t<decltype(instance)>
		> && ...))
	{
		using T = std::remove_cvref_t<std::tuple_element_t<0, std::tuple<decltype(instance)...>>>;
		using CountT = decltype(reflection_traits<T>::member_count);
		auto loop {[&, ...instance = std::forward<decltype(instance)> (instance)]
			<std::unsigned_integral auto I = CountT{0}> (auto& loop) mutable
		{
			using Member = typename std::tuple_element<I, typename reflection_traits<T>::member_types>::type;
			if constexpr (!flex::is_specialization_of<Member, WriteOnly>::value) {
				using FuncRet = std::invoke_result_t<
					decltype(func),
					std::add_lvalue_reference_t<std::tuple_element_t<I, typename reflection_traits<
						std::remove_cvref_t<decltype(instance)>
					>::member_types>>...
				>;
				if constexpr (std::is_void<FuncRet>::value)
					func(internals::unwrapMember(reflection_traits<T>::template getMember<I> (instance))...);
				else if constexpr (std::same_as<FuncRet, bool>) {
					if (!func(internals::unwrapMember(reflection_traits<T>::template getMember<I> (instance))...))
						return;
				}
				else
					flex::comptimeErrorWithTypes<FuncRet> ("Can't have callback with the given return type");
			}
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
			using Member = typename std::tuple_element<I, typename reflection_traits<T>::member_types>::type;
			if constexpr (!flex::is_specialization_of<Member, WriteOnly>::value) {
				using FuncRet = std::invoke_result_t<
					decltype(func),
					std::add_lvalue_reference_t<std::tuple_element_t<I, typename reflection_traits<T>::member_types>>,
					std::string_view
				>;
				if constexpr (std::is_void<FuncRet>::value) {
					func(
						internals::unwrapMember(reflection_traits<T>::template getMember<I> (instance)),
						reflection_traits<T>::member_names[I]
					);
				}
				else if constexpr (std::same_as<FuncRet, bool>) {
					if (!func(
						internals::unwrapMember(reflection_traits<T>::template getMember<I> (instance)),
						reflection_traits<T>::member_names[I]
					))
						return;
				}
				else
					flex::comptimeErrorWithTypes<FuncRet> ("Can't have callback with the given return type");
			}
			if constexpr (I + 1 < reflection_traits<T>::member_count)
				loop.template operator() <I + 1> (loop);
		}};
		loop(loop);
	}
}


namespace flex {
	template <flex::reflection::reflectable T>
	struct Stringifier<T> {
		constexpr auto operator() (
			flex::forward_of<T> auto&& value,
			const flex::StringifyStyle& style = {}
		) const noexcept -> std::string {
			std::string_view prefix {};
			std::string indent {};
			std::string result {"{"};
			if (style.prettify) {
				prefix = "\n";
				indent = std::string(style.currentIndent + style.indentSize, ' ');
			}

			flex::reflection::foreachNamedMember(std::forward<decltype(value)> (value), [
				&style, &prefix, &result, &indent
			](
				auto& member, std::string_view name
			) {
				const std::string memberValueAsString {flex::internals::reflectedValueToString(member, style)};
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

