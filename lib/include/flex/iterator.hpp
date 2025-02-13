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

	} // namespace __internals


	template <typename T>
	struct iterator_traits {
		using category = typename __internals::iterator_category_detector<T>::type;
	};


	template <typename T, typename = void>
	struct is_iterator : std::false_type {};

	template <typename T>
	struct is_iterator<T, flex::make_void_t<iterator_traits<T>>> : std::true_type {};

	template <typename T>
	constexpr auto is_iterator_v {flex::is_iterator<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept iterator = is_iterator_v<T>;
#endif


	template <typename T, typename Category>
	struct is_iterator_category : std::bool_constant<
		is_iterator_v<T>
		&& std::is_base_of_v<Category, typename iterator_traits<T>::category>
	> {};

	template <typename T, typename Category>
	constexpr auto is_iterator_category_v {is_iterator_category<T, Category>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T, typename Category>
	concept iterator_category = is_iterator_category_v<T, Category>;
#endif

} // namespace flex
