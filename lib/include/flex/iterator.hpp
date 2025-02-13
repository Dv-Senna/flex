#pragma once

#include "flex/config.hpp"

#include <iterator>
#include <type_traits>

#include "flex/typeTraits.hpp"


namespace flex {
	namespace iteratorCategory {
		struct Input {};
		struct Forward : Input {};
		struct Bidirectional : Forward {};
		struct RandomAccess : Bidirectional {};
		struct Contiguous : RandomAccess {};
	} // namespace iteratorCategory


	namespace __internals {
		template <typename T>
		struct std_iterator_category_converter {};

		template <> struct std_iterator_category_converter<std::input_iterator_tag> {using type = iteratorCategory::Input;};
		template <> struct std_iterator_category_converter<std::forward_iterator_tag> {using type = iteratorCategory::Forward;};
		template <> struct std_iterator_category_converter<std::bidirectional_iterator_tag> {using type = iteratorCategory::Bidirectional;};
		template <> struct std_iterator_category_converter<std::random_access_iterator_tag> {using type = iteratorCategory::RandomAccess;};
	#if FLEX_CONFIG_CPP_STANDARD >= 20	
		template <> struct std_iterator_category_converter<std::contiguous_iterator_tag> {using type = iteratorCategory::Contiguous;};
	#endif

		template <typename T, typename = void>
		struct iterator_category_detector {};

		template <typename T>
		struct iterator_category_detector<T,
			flex::make_void_t<typename T::IteratorCategory>
		> {
			using type = typename T::IteratorCategory;
		};

		template <typename T>
		struct iterator_category_detector<T,
			flex::make_void_t<typename T::iterator_category>
		> {
			using type = typename std_iterator_category_converter<typename T::iterator_category>::type;
		};


		template <typename T, typename = void>
		struct iterator_arithmetic_type_detector {};

		template <typename T>
		struct iterator_arithmetic_type_detector<T,
			flex::make_void_t<typename T::ArithmeticType>
		> {
			using type = typename T::ArithmeticType;
		};

		template <typename T>
		struct iterator_arithmetic_type_detector<T,
			flex::make_void_t<typename T::difference_type>
		> {
			using type = typename T::difference_type;
		};


		template <typename T, typename = void>
		struct iterator_value_type_detector {};

		template <typename T>
		struct iterator_value_type_detector<T,
			flex::make_void_t<typename T::ValueType>
		> {
			using type = typename T::ValueType;
		};

		template <typename T>
		struct iterator_value_type_detector<T,
			flex::make_void_t<typename T::value_type>
		> {
			using type = typename T::value_type;
		};


		template <typename T, typename = void>
		struct iterator_reference_type_detector {
			using type = std::add_lvalue_reference_t<typename iterator_value_type_detector<T>::type>;
		};

		template <typename T>
		struct iterator_reference_type_detector<T,
			flex::make_void_t<typename T::ReferenceType>
		> {
			using type = typename T::ReferenceType;
		};

		template <typename T>
		struct iterator_reference_type_detector<T,
			flex::make_void_t<typename T::reference>
		> {
			using type = typename T::reference;
		};


		template <typename T, typename = void>
		struct iterator_pointer_type_detector {
			using type = std::add_pointer_t<typename iterator_value_type_detector<T>::type>;
		};

		template <typename T>
		struct iterator_pointer_type_detector<T,
			flex::make_void_t<typename T::PointerType>
		> {
			using type = typename T::PointerType;
		};

		template <typename T>
		struct iterator_pointer_type_detector<T,
			flex::make_void_t<typename T::pointer>
		> {
			using type = typename T::pointer;
		};

	} // namespace __internals


	/*
	 * @brief The type traits used to interface with unknown iterator
	 *
	 * @types Category The category of the iterator. Taken from T::IteratorCategory or
	 *                 deduced from T::iterator_category. See flex::iteratorCategory for
	 *                 more details
	 * @types ArithmeticType The type representing scalars. Taken from T::ArithmeticType
	 *                       or from T::difference_type
	 * @types ValueType The type of the underlying value. Taken from T::ValueType or from
	 *                  T::value_type
	 * @types ReferenceType The type of reference to the value. Taken from
	 *                      T::ReferenceType or from T::reference. If not available,
	 *                      generated as std::add_lvalue_reference_t<ValueType>
	 * @types PointerType The type of pointer to the value. Taken from T::PointerType or
	 *                    from T::pointer. If not availble, generated as std::add_pointer_t<ValueType>
	 * */
	template <typename T>
	struct iterator_traits {
		using Category = typename __internals::iterator_category_detector<T>::type;
		using ArithmeticType = typename __internals::iterator_arithmetic_type_detector<T>::type;
		using ValueType = typename __internals::iterator_value_type_detector<T>::type;
		using ReferenceType = typename __internals::iterator_reference_type_detector<T>::type;
		using PointerType = typename __internals::iterator_pointer_type_detector<T>::type;
	};


	template <typename T, typename = void>
	struct is_iterator : std::false_type {};

	template <typename T>
	struct is_iterator<T, flex::make_void_t<iterator_traits<T>>> : std::true_type {};

	template <typename T>
	constexpr auto is_iterator_v {flex::is_iterator<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept iterator = is_iterator_v<T> && requires() {
		typename iterator_traits<T>::Category;
		typename iterator_traits<T>::ArithmeticType;
		typename iterator_traits<T>::ValueType;
		typename iterator_traits<T>::ReferenceType;
		typename iterator_traits<T>::PointerType;
	};
	#define FLEX_TYPENAME_ITERATOR ::flex::iterator
#else
	#define FLEX_TYPENAME_ITERATOR typename
#endif


	namespace __internals {
		template <typename T, typename Category>
		struct has_iterator_category : std::bool_constant<
			is_iterator_v<T>
			&& std::is_base_of_v<Category, typename iterator_traits<T>::Category>
		> {};

	} // namespace __internals


	template <typename T, typename Category, typename = void>
	struct is_iterator_category : std::false_type {};

	template <typename T>
	struct is_iterator_category<T, iteratorCategory::Contiguous,
		std::enable_if_t<
			__internals::has_iterator_category<T, iteratorCategory::Contiguous>::value
			&& is_iterator_category<T, iteratorCategory::RandomAccess>::value
		, void>
	> : std::true_type {};

	template <typename T>
	struct is_iterator_category<T, iteratorCategory::RandomAccess,
		std::enable_if_t<
			__internals::has_iterator_category<T, iteratorCategory::RandomAccess>::value
			&& is_iterator_category<T, iteratorCategory::Bidirectional>::value
			&& flex::is_comparable_v<T>
			&& std::is_same_v<
				decltype(std::declval<T> () += std::declval<std::add_const_t<typename iterator_traits<T>::ArithmeticType>> ()),
				std::add_lvalue_reference_t<T>
			>
			&& std::is_same_v<
				decltype(std::declval<T> () -= std::declval<std::add_const_t<typename iterator_traits<T>::ArithmeticType>> ()),
				std::add_lvalue_reference_t<T>
			>
			&& std::is_same_v<
				decltype(std::declval<std::add_const_t<T>> () + std::declval<std::add_const_t<typename iterator_traits<T>::ArithmeticType>> ()),
				T
			>
			&& std::is_same_v<
				decltype(std::declval<std::add_const_t<typename iterator_traits<T>::ArithmeticType>> () + std::declval<std::add_const_t<T>> ()),
				T
			>
			&& std::is_same_v<
				decltype(std::declval<std::add_const_t<T>> () - std::declval<std::add_const_t<typename iterator_traits<T>::ArithmeticType>> ()),
				T
			>
			&& std::is_same_v<
				decltype(std::declval<std::add_const_t<T>> () - std::declval<std::add_const_t<T>> ()),
				typename iterator_traits<T>::ArithmeticType
			>
			&& std::is_same_v<
				decltype(std::declval<std::add_const_t<T>> ()[std::declval<std::add_const_t<typename iterator_traits<T>::ArithmeticType>> ()]),
				typename iterator_traits<T>::ReferenceType
			>
		, void>
	> : std::true_type {};

	template <typename T>
	struct is_iterator_category<T, iteratorCategory::Bidirectional,
		std::enable_if_t<
			__internals::has_iterator_category<T, iteratorCategory::Bidirectional>::value
			&& is_iterator_category<T, iteratorCategory::Forward>::value
			&& std::is_same_v<decltype(--std::declval<T> ()), std::add_lvalue_reference_t<T>>
			&& std::is_same_v<decltype(std::declval<T> ()--), T>
		, void>
	> : std::true_type {};

	template <typename T>
	struct is_iterator_category<T, iteratorCategory::Forward,
		std::enable_if_t<
			__internals::has_iterator_category<T, iteratorCategory::Forward>::value
			&& is_iterator_category<T, iteratorCategory::Input>::value
			&& flex::is_comparable_v<typename iterator_traits<T>::ArithmeticType>
			&& (std::is_copy_constructible_v<typename iterator_traits<T>::ArithmeticType>
				|| std::is_move_constructible_v<typename iterator_traits<T>::ArithmeticType>)
			&& std::is_same_v<decltype(std::declval<T> ()++), T>
		, void>
	> : std::true_type {};

	template <typename T>
	struct is_iterator_category<T, iteratorCategory::Input,
		std::enable_if_t<
			__internals::has_iterator_category<T, iteratorCategory::Input>::value
			&& std::is_default_constructible_v<T>
			&& std::is_copy_constructible_v<T>
			&& std::is_copy_assignable_v<T>
			&& flex::is_equality_comparable_v<T>
			&& std::is_same_v<decltype(*std::declval<std::add_const_t<T>> ()), typename iterator_traits<T>::ReferenceType>
			&& std::is_same_v<decltype(++std::declval<T> ()), std::add_lvalue_reference_t<T>>
		, void>
	> : std::true_type {};

	template <typename T, typename Category>
	constexpr auto is_iterator_category_v {is_iterator_category<T, Category>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T, typename Category>
	concept iterator_category = is_iterator_category_v<T, Category>;
	#define FLEX_TYPENAME_ITERATOR_CATEGORY(Category) ::flex::iterator_category<Category>
#else
	#define FLEX_TYPENAME_ITERATOR_CATEGORY(Category) typename
#endif

	
	template <typename T>
	struct is_input_iterator : std::bool_constant<flex::is_iterator_category_v<T, iteratorCategory::Input>> {};
	template <typename T>
	struct is_forward_iterator : std::bool_constant<flex::is_iterator_category_v<T, iteratorCategory::Forward>> {};
	template <typename T>
	struct is_bidirectional_iterator : std::bool_constant<flex::is_iterator_category_v<T, iteratorCategory::Bidirectional>> {};
	template <typename T>
	struct is_random_access_iterator : std::bool_constant<flex::is_iterator_category_v<T, iteratorCategory::RandomAccess>> {};
	template <typename T>
	struct is_contiguous_iterator : std::bool_constant<flex::is_iterator_category_v<T, iteratorCategory::Contiguous>> {};

	template <typename T>
	constexpr auto is_input_iterator_v {flex::is_input_iterator<T>::value};
	template <typename T>
	constexpr auto is_forward_iterator_v {flex::is_forward_iterator<T>::value};
	template <typename T>
	constexpr auto is_bidirectional_iterator_v {flex::is_bidirectional_iterator<T>::value};
	template <typename T>
	constexpr auto is_random_access_iterator_v {flex::is_random_access_iterator<T>::value};
	template <typename T>
	constexpr auto is_contiguous_iterator_v {flex::is_contiguous_iterator<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept input_iterator = is_input_iterator_v<T> && requires(
		T val,
		std::add_const_t<T> cst
	) {
		{*val} -> std::same_as<typename iterator_traits<T>::ReferenceType>;
		{++val} -> std::same_as<std::add_lvalue_reference_t<T>>;
	}
		&& flex::equality_comparable<T>
		&& flex::iterator<T>;

	template <typename T>
	concept forward_iterator = is_forward_iterator_v<T> && requires(T val) {
		{val++} -> std::same_as<T>;
	}
		&& flex::comparable<typename iterator_traits<T>::ArithmeticType>
		&& flex::input_iterator<T>;

	template <typename T>
	concept bidirectional_iterator = is_bidirectional_iterator_v<T> && requires(T val) {
		{--val} -> std::same_as<std::add_lvalue_reference_t<T>>;
		{val--} -> std::same_as<T>;
	}
		&& flex::forward_iterator<T>;

	template <typename T>
	concept random_access_iterator = is_random_access_iterator_v<T> && requires(
		T val,
		std::add_const_t<T> cst,
		std::add_const_t<typename iterator_traits<T>::ArithmeticType> n
	) {
		{val += n} -> std::same_as<std::add_lvalue_reference_t<T>>;
		{val -= n} -> std::same_as<std::add_lvalue_reference_t<T>>;
		{cst + n} -> std::same_as<T>;
		{cst - n} -> std::same_as<T>;
		{n + cst} -> std::same_as<T>;
		{cst - cst} -> std::same_as<typename iterator_traits<T>::ArithmeticType>;
		{cst[n]} -> std::same_as<typename iterator_traits<T>::ReferenceType>;
	}
		&& flex::bidirectional_iterator<T>;

	template <typename T>
	concept contiguous_iterator = is_contiguous_iterator_v<T>
		&& flex::random_access_iterator<T>;

	#define FLEX_TYPENAME_INPUT_ITERATOR ::flex::input_iterator
	#define FLEX_TYPENAME_FORWARD_ITERATOR ::flex::forward_iterator
	#define FLEX_TYPENAME_BIDIRECTIONAL_ITERATOR ::flex::bidirectional_iterator
	#define FLEX_TYPENAME_RANDOM_ACCESS_ITERATOR ::flex::random_access_iterator
	#define FLEX_TYPENAME_CONTIGUOUS_ITERATOR ::flex::contiguous_iterator
#else
	#define FLEX_TYPENAME_INPUT_ITERATOR typename
	#define FLEX_TYPENAME_FORWARD_ITERATOR typename
	#define FLEX_TYPENAME_BIDIRECTIONAL_ITERATOR typename
	#define FLEX_TYPENAME_RANDOM_ACCESS_ITERATOR typename
	#define FLEX_TYPENAME_CONTIGUOUS_ITERATOR typename
#endif

} // namespace flex
