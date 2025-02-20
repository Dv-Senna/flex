#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>


namespace flex {
	template <typename T>
	concept enumeration = std::is_enum_v<T>;

	template <enumeration T>
	struct enum_max_size : std::integral_constant<std::size_t, FLEX_REFLECTION_MAX_ENUM_SIZE> {};

	template <enumeration T>
	constexpr auto enum_max_size_v = enum_max_size<T>::value;


	namespace __internals {
		template <enumeration T, T VALUE>
		consteval auto getEnumName() noexcept -> std::optional<std::string_view>;

	} // namespace __internals


	template <enumeration T>
	struct enum_value_generator {
		static constexpr auto value {[](std::size_t val) constexpr {return static_cast<T> (val);}};
		using value_type = decltype(value);
	};

	template <enumeration T>
	constexpr auto enum_value_generator_v = enum_value_generator<T>::value;


	template <enumeration T, T VALUE, typename = void>
	struct is_enum_member : std::false_type {};

	template <enumeration T, T VALUE>
	struct is_enum_member<T, VALUE,
		std::enable_if_t<!!__internals::getEnumName<T, VALUE> (), void>
	> : std::true_type {};

	template <enumeration T, T VALUE>
	constexpr auto is_enum_member_v = is_enum_member<T, VALUE>::value;


	template <enumeration T>
	struct PackedEnumName {
		T value;
		std::string_view name;
	};


	namespace __internals {
		template <
			enumeration T,
			std::size_t INDEX = 0,
			bool IS_MEMBER = is_enum_member_v<T, enum_value_generator_v<T> (INDEX)>,
			std::size_t MAX_INDEX = enum_max_size_v<T>
		>
		struct enum_members_generator {
			static constexpr std::tuple value {std::tuple_cat(
				std::make_tuple(PackedEnumName<T> {enum_value_generator_v<T> (INDEX), *__internals::getEnumName<T, enum_value_generator_v<T> (INDEX)> ()}),
				enum_members_generator<T, INDEX+1>::value
			)};
			using value_type = decltype(value);
		};

		template <enumeration T, std::size_t INDEX, std::size_t MAX_INDEX>
		struct enum_members_generator<T, INDEX, false, MAX_INDEX> {
			static constexpr std::tuple value {enum_members_generator<T, INDEX+1>::value};
		};

		template <enumeration T, std::size_t MAX_INDEX>
		struct enum_members_generator<T, MAX_INDEX, true, MAX_INDEX> {
			static constexpr std::tuple value {};
			using value_type = decltype(value);
		};

		template <enumeration T, std::size_t MAX_INDEX>
		struct enum_members_generator<T, MAX_INDEX, false, MAX_INDEX> {
			static constexpr std::tuple value {};
			using value_type = decltype(value);
		};
		
		template <enumeration T>
		constexpr auto enum_members_generator_v = enum_members_generator<T>::value;


		template <enumeration T>
		constexpr auto enumTupleToArray() {
			constexpr auto getArray {[](auto &&...values) constexpr {return std::array{std::forward<decltype(values)> (values)...};}};
			return std::apply(getArray, enum_members_generator_v<T>);
		}

	} // namespace __internals


	template <enumeration T>
	struct enum_members {
		static constexpr auto value {__internals::enumTupleToArray<T> ()};
		using value_type = decltype(value);
	};

	template <enumeration T>
	constexpr auto enum_members_v = enum_members<T>::value;


	template <enumeration T>
	constexpr auto enum_members_count_v = enum_members_v<T>.size();


	template <enumeration T>
	constexpr auto toString(T value) noexcept -> std::optional<std::string_view>;


} // namespace flex


#define FLEX_MAKE_ENUM_BITFLAG(name) template <>\
	struct flex::enum_value_generator<SomeBitfield> {\
		static constexpr auto value {[](std::size_t index) constexpr {return (SomeBitfield)(1 << index);}};\
		using value_type = decltype(value);\
	}
#define FLEX_SET_MAX_ENUM_SIZE(name, value) template <>\
	struct flex::enum_max_size<SomeBitfield> : std::integral_constant<std::size_t, 3> {}


#include "flex/reflection/enums.inl"
