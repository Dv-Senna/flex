#pragma once

#include "flex/config.hpp"

#include <iterator>
#include <type_traits>
#include <vector>

#include "flex/typeTraits.hpp"


namespace flex {
	enum IteratorCategory {
		/*
		 * An iterator such that its value may not exist anymore after increment
		 * */
		eInput,
		/*
		 * An iterator that can only move forward, one step at a time
		 * */
		eForward,
		/*
		 * An iterator that can move forward and backward, one step at a time
		 * */
		eBidirectional,
		/*
		 * An iterator that can access everywhere in its container in constant time
		 * */
		eRandomAccess,
		/*
		 * An iterator whose container's underlying datas are stored continously, in the
		 * same order they're accessed by the iterator
		 * */
		eContiguous
	};

	static_assert(IteratorCategory::eInput < IteratorCategory::eForward);
	static_assert(IteratorCategory::eForward < IteratorCategory::eBidirectional);
	static_assert(IteratorCategory::eBidirectional < IteratorCategory::eRandomAccess);
	static_assert(IteratorCategory::eRandomAccess < IteratorCategory::eContiguous);


	namespace __internals {
		template <typename T>
		struct std_iterator_category_converter {};

		template <> struct std_iterator_category_converter<std::input_iterator_tag> {
			static constexpr auto value {IteratorCategory::eInput};
		};
		template <> struct std_iterator_category_converter<std::forward_iterator_tag> {
			static constexpr auto value{IteratorCategory::eForward};
		};
		template <> struct std_iterator_category_converter<std::bidirectional_iterator_tag> {
			static constexpr auto value{IteratorCategory::eBidirectional};
		};
		template <> struct std_iterator_category_converter<std::random_access_iterator_tag> {
			static constexpr auto value{IteratorCategory::eRandomAccess};
		};
	#if FLEX_CONFIG_CPP_STANDARD >= 20	
		template <> struct std_iterator_category_converter<std::contiguous_iterator_tag> {
			static constexpr auto value{IteratorCategory::eContiguous};
		};
	#endif

		template <typename T, typename = void>
		struct iterator_category_detector {};

		template <typename T>
		struct iterator_category_detector<T,
			flex::make_void_t<decltype(T::ITERATOR_CATEGORY)>
		> {
			static constexpr auto value {T::ITERATOR_CATEGORY};
		};

		template <typename T>
		struct iterator_category_detector<T,
			flex::make_void_t<typename T::iterator_category>
		> {
			static constexpr auto value {std_iterator_category_converter<typename T::iterator_category>::value};
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
	 *                 deduced from T::iterator_category. See flex::IteratorCategory for
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
		static constexpr auto CATEGORY {__internals::iterator_category_detector<T>::value};
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
		iterator_traits<T>::CATEGORY;
		typename iterator_traits<T>::ArithmeticType;
		typename iterator_traits<T>::ValueType;
		typename iterator_traits<T>::ReferenceType;
		typename iterator_traits<T>::PointerType;
	};
	#define FLEX_TYPENAME_ITERATOR ::flex::iterator
#else
	#define FLEX_TYPENAME_ITERATOR typename
#endif


	template <typename T, typename = void>
	struct is_std_iterator : std::false_type {};

	template <typename T>
	struct is_std_iterator<T,
		std::enable_if_t<
			flex::true_v<typename T::iterator_category>
			&& is_iterator_v<T>
		, void>
	> : std::true_type {};

	template <typename T>
	constexpr auto is_std_iterator_v {is_std_iterator<T>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T>
	concept std_iterator = is_std_iterator_v<T>;
	#define FLEX_TYPENAME_STD_ITERATOR ::flex::std_iterator
#else
	#define FLEX_TYPENAME_STD_ITERATOR typename
#endif


	namespace __internals {
		template <typename T, IteratorCategory CATEGORY>
		struct has_iterator_category : std::bool_constant<
			is_iterator_v<T>
			&& iterator_traits<T>::CATEGORY >= CATEGORY
		> {};

	} // namespace __internals


	template <typename T, IteratorCategory CATEGORY, typename = void>
	struct is_iterator_category : std::false_type {};

	template <typename T>
	struct is_iterator_category<T, IteratorCategory::eContiguous,
		std::enable_if_t<
			__internals::has_iterator_category<T, IteratorCategory::eContiguous>::value
			&& is_iterator_category<T, IteratorCategory::eRandomAccess>::value
		, void>
	> : std::true_type {};

	template <typename T>
	struct is_iterator_category<T, IteratorCategory::eRandomAccess,
		std::enable_if_t<
			__internals::has_iterator_category<T, IteratorCategory::eRandomAccess>::value
			&& is_iterator_category<T, IteratorCategory::eBidirectional>::value
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
	struct is_iterator_category<T, IteratorCategory::eBidirectional,
		std::enable_if_t<
			__internals::has_iterator_category<T, IteratorCategory::eBidirectional>::value
			&& is_iterator_category<T, IteratorCategory::eForward>::value
			&& std::is_same_v<decltype(--std::declval<T> ()), std::add_lvalue_reference_t<T>>
			&& std::is_same_v<decltype(std::declval<T> ()--), T>
		, void>
	> : std::true_type {};

	template <typename T>
	struct is_iterator_category<T, IteratorCategory::eForward,
		std::enable_if_t<
			__internals::has_iterator_category<T, IteratorCategory::eForward>::value
			&& is_iterator_category<T, IteratorCategory::eInput>::value
			&& flex::is_comparable_v<typename iterator_traits<T>::ArithmeticType>
			&& std::is_default_constructible_v<typename iterator_traits<T>::ArithmeticType>
			&& (std::is_copy_constructible_v<typename iterator_traits<T>::ArithmeticType>
				|| std::is_move_constructible_v<typename iterator_traits<T>::ArithmeticType>)
			&& std::same_as<decltype(-std::declval<std::add_const_t<typename iterator_traits<T>::ArithmeticType>> ()), typename iterator_traits<T>::ArithmeticType>
			&& std::is_same_v<decltype(std::declval<T> ()++), T>
		, void>
	> : std::true_type {};

	template <typename T>
	struct is_iterator_category<T, IteratorCategory::eInput,
		std::enable_if_t<
			__internals::has_iterator_category<T, IteratorCategory::eInput>::value
			&& std::is_default_constructible_v<T>
			&& std::is_copy_constructible_v<T>
			&& std::is_copy_assignable_v<T>
			&& flex::is_equality_comparable_v<T>
			&& std::is_same_v<decltype(*std::declval<std::add_const_t<T>> ()), typename iterator_traits<T>::ReferenceType>
			&& std::is_same_v<decltype(++std::declval<T> ()), std::add_lvalue_reference_t<T>>
		, void>
	> : std::true_type {};

	template <typename T, IteratorCategory CATEGORY>
	constexpr auto is_iterator_category_v {is_iterator_category<T, CATEGORY>::value};

#if defined(FLEX_CONFIG_CPP_FEATURES_CONCEPTS)
	template <typename T, IteratorCategory CATEGORY>
	concept iterator_category = is_iterator_category_v<T, CATEGORY>;
	#define FLEX_TYPENAME_ITERATOR_CATEGORY(CATEGORY) ::flex::iterator_category<CATEGORY>
#else
	#define FLEX_TYPENAME_ITERATOR_CATEGORY(CATEGORY) typename
#endif

	
	template <typename T>
	struct is_input_iterator : std::bool_constant<flex::is_iterator_category_v<T, IteratorCategory::eInput>> {};
	template <typename T>
	struct is_forward_iterator : std::bool_constant<flex::is_iterator_category_v<T, IteratorCategory::eForward>> {};
	template <typename T>
	struct is_bidirectional_iterator : std::bool_constant<flex::is_iterator_category_v<T, IteratorCategory::eBidirectional>> {};
	template <typename T>
	struct is_random_access_iterator : std::bool_constant<flex::is_iterator_category_v<T, IteratorCategory::eRandomAccess>> {};
	template <typename T>
	struct is_contiguous_iterator : std::bool_constant<flex::is_iterator_category_v<T, IteratorCategory::eContiguous>> {};

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
		&& flex::comparable<T>
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


	namespace __internals {
		template <typename T, IteratorCategory CATEGORY>
		class std_iterator_wrapper {};

		template <typename T>
		class std_iterator_wrapper<T, IteratorCategory::eInput> {
			using This = std_iterator_wrapper<T, IteratorCategory::eInput>;

			public:
				static constexpr auto ITERATOR_CATEGORY {IteratorCategory::eInput};
				using ArithmeticType = typename iterator_traits<T>::ArithmeticType;
				using ValueType = typename iterator_traits<T>::ValueType;
				using ReferenceType = typename iterator_traits<T>::ReferenceType;
				using PointerType = typename iterator_traits<T>::PointerType;

				constexpr std_iterator_wrapper() noexcept = default;
				constexpr ~std_iterator_wrapper() = default;

				constexpr std_iterator_wrapper(const T &it) noexcept : m_it {it} {}

				constexpr std_iterator_wrapper(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr std_iterator_wrapper(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto operator==(const This &it) const noexcept {return m_it == it.m_it;}
				constexpr auto operator++() noexcept -> This& {++m_it; return *this;}
				constexpr auto operator*() const noexcept -> ValueType& {return *m_it;}

			protected:
				T m_it;
		};

		template <typename T>
		class std_iterator_wrapper<T, IteratorCategory::eForward> {
			using This = std_iterator_wrapper<T, IteratorCategory::eForward>;

			public:
				static constexpr auto ITERATOR_CATEGORY {IteratorCategory::eForward};
				using ArithmeticType = typename iterator_traits<T>::ArithmeticType;
				using ValueType = typename iterator_traits<T>::ValueType;
				using ReferenceType = typename iterator_traits<T>::ReferenceType;
				using PointerType = typename iterator_traits<T>::PointerType;

				constexpr std_iterator_wrapper() noexcept = default;
				constexpr ~std_iterator_wrapper() = default;

				constexpr std_iterator_wrapper(const T &it) noexcept : m_it {it} {}

				constexpr std_iterator_wrapper(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr std_iterator_wrapper(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto operator==(const This &it) const noexcept {return m_it == it.m_it;}

				constexpr auto operator++() noexcept -> This& {++m_it; return *this;}
				constexpr auto operator++(int) noexcept -> This {auto tmp {*this}; ++m_it; return tmp;}
				constexpr auto operator*() const noexcept -> ValueType& {return *m_it;}

			private:
				T m_it;
		};

		template <typename T>
		class std_iterator_wrapper<T, IteratorCategory::eBidirectional> {
			using This = std_iterator_wrapper<T, IteratorCategory::eBidirectional>;

			public:
				static constexpr auto ITERATOR_CATEGORY {IteratorCategory::eBidirectional};
				using ArithmeticType = typename iterator_traits<T>::ArithmeticType;
				using ValueType = typename iterator_traits<T>::ValueType;
				using ReferenceType = typename iterator_traits<T>::ReferenceType;
				using PointerType = typename iterator_traits<T>::PointerType;

				constexpr std_iterator_wrapper() noexcept = default;
				constexpr ~std_iterator_wrapper() = default;

				constexpr std_iterator_wrapper(const T &it) noexcept : m_it {it} {}

				constexpr std_iterator_wrapper(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr std_iterator_wrapper(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto operator==(const This &it) const noexcept {return m_it == it.m_it;}

				constexpr auto operator++() noexcept -> This& {++m_it; return *this;}
				constexpr auto operator++(int) noexcept -> This {auto tmp {*this}; ++m_it; return tmp;}
				constexpr auto operator--() noexcept -> This& {--m_it; return *this;}
				constexpr auto operator--(int) noexcept -> This {auto tmp {*this}; --m_it; return tmp;}

				constexpr auto operator*() const noexcept -> ValueType& {return *m_it;}

			private:
				T m_it;
		};

		template <typename T>
		class std_iterator_wrapper<T, IteratorCategory::eRandomAccess> {
			using This = std_iterator_wrapper<T, IteratorCategory::eRandomAccess>;

			public:
				static constexpr auto ITERATOR_CATEGORY {IteratorCategory::eRandomAccess};
				using ArithmeticType = typename iterator_traits<T>::ArithmeticType;
				using ValueType = typename iterator_traits<T>::ValueType;
				using ReferenceType = typename iterator_traits<T>::ReferenceType;
				using PointerType = typename iterator_traits<T>::PointerType;

				constexpr std_iterator_wrapper() noexcept = default;
				constexpr ~std_iterator_wrapper() = default;

				constexpr std_iterator_wrapper(const T &it) noexcept : m_it {it} {}

				constexpr std_iterator_wrapper(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr std_iterator_wrapper(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto operator==(const This &it) const noexcept {return m_it == it.m_it;}
				constexpr auto operator<(const This &it) const noexcept -> bool {return m_it < it.m_it;}
				constexpr auto operator>(const This &it) const noexcept -> bool {return m_it > it.m_it;}
				constexpr auto operator<=(const This &it) const noexcept -> bool {return m_it <= it.m_it;}
				constexpr auto operator>=(const This &it) const noexcept -> bool {return m_it >= it.m_it;}

				constexpr auto operator++() noexcept -> This& {++m_it; return *this;}
				constexpr auto operator++(int) noexcept -> This {auto tmp {*this}; ++m_it; return tmp;}
				constexpr auto operator--() noexcept -> This& {--m_it; return *this;}
				constexpr auto operator--(int) noexcept -> This {auto tmp {*this}; --m_it; return tmp;}

				constexpr auto operator+=(const ArithmeticType &n) noexcept -> This& {m_it += n; return *this;}
				constexpr auto operator-=(const ArithmeticType &n) noexcept -> This& {m_it += n; return *this;}
				constexpr auto operator+(const ArithmeticType &n) const noexcept -> This {auto tmp {*this}; return tmp += n;}
				constexpr auto operator-(const ArithmeticType &n) const noexcept -> This {auto tmp {*this}; return tmp -= n;}
				constexpr auto operator-(const This &it) const noexcept -> ArithmeticType {return m_it - it.m_it;}

				constexpr auto operator*() const noexcept -> ValueType& {return *m_it;}
				constexpr auto operator[](const ArithmeticType &n) const noexcept -> ReferenceType {return m_it[n];}

			private:
				T m_it;
		};

		template <typename T>
		constexpr auto operator+(
			const typename std_iterator_wrapper<T, IteratorCategory::eRandomAccess>::ArithmeticType &n,
			const std_iterator_wrapper<T, IteratorCategory::eRandomAccess> &it
		) noexcept -> std_iterator_wrapper<T, IteratorCategory::eRandomAccess> {
			return it + n;
		}

		template <typename T>
		class std_iterator_wrapper<T, IteratorCategory::eContiguous> {
			using This = std_iterator_wrapper<T, IteratorCategory::eContiguous>;

			public:
				static constexpr auto ITERATOR_CATEGORY {IteratorCategory::eContiguous};
				using ArithmeticType = typename iterator_traits<T>::ArithmeticType;
				using ValueType = typename iterator_traits<T>::ValueType;
				using ReferenceType = typename iterator_traits<T>::ReferenceType;
				using PointerType = typename iterator_traits<T>::PointerType;

				constexpr std_iterator_wrapper() noexcept = default;
				constexpr ~std_iterator_wrapper() = default;

				constexpr std_iterator_wrapper(const T &it) noexcept : m_it {it} {}

				constexpr std_iterator_wrapper(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr std_iterator_wrapper(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto operator==(const This &it) const noexcept {return m_it == it.m_it;}
				constexpr auto operator<(const This &it) const noexcept -> bool {return m_it < it.m_it;}
				constexpr auto operator>(const This &it) const noexcept -> bool {return m_it > it.m_it;}
				constexpr auto operator<=(const This &it) const noexcept -> bool {return m_it <= it.m_it;}
				constexpr auto operator>=(const This &it) const noexcept -> bool {return m_it >= it.m_it;}

				constexpr auto operator++() noexcept -> This& {++m_it; return *this;}
				constexpr auto operator++(int) noexcept -> This {auto tmp {*this}; ++m_it; return tmp;}
				constexpr auto operator--() noexcept -> This& {--m_it; return *this;}
				constexpr auto operator--(int) noexcept -> This {auto tmp {*this}; --m_it; return tmp;}

				constexpr auto operator+=(const ArithmeticType &n) noexcept -> This& {m_it += n; return *this;}
				constexpr auto operator-=(const ArithmeticType &n) noexcept -> This& {m_it += n; return *this;}
				constexpr auto operator+(const ArithmeticType &n) const noexcept -> This {auto tmp {*this}; return tmp += n;}
				constexpr auto operator-(const ArithmeticType &n) const noexcept -> This {auto tmp {*this}; return tmp -= n;}
				constexpr auto operator-(const This &it) const noexcept -> ArithmeticType {return m_it - it.m_it;}

				constexpr auto operator*() const noexcept -> ValueType& {return *m_it;}
				constexpr auto operator[](const ArithmeticType &n) const noexcept -> ReferenceType {return m_it[n];}

			private:
				T m_it;
		};

		template <typename T>
		constexpr auto operator+(
			const typename std_iterator_wrapper<T, IteratorCategory::eContiguous>::ArithmeticType &n,
			const std_iterator_wrapper<T, IteratorCategory::eContiguous> &it
		) noexcept -> std_iterator_wrapper<T, IteratorCategory::eContiguous> {
			return it + n;
		}

		static_assert(is_input_iterator_v<std_iterator_wrapper<std::vector<int>::iterator, IteratorCategory::eInput>>);
		static_assert(is_forward_iterator_v<std_iterator_wrapper<std::vector<int>::iterator, IteratorCategory::eForward>>);
		static_assert(is_bidirectional_iterator_v<std_iterator_wrapper<std::vector<int>::iterator, IteratorCategory::eBidirectional>>);
		static_assert(is_random_access_iterator_v<std_iterator_wrapper<std::vector<int>::iterator, IteratorCategory::eRandomAccess>>);
		static_assert(is_contiguous_iterator_v<std_iterator_wrapper<std::vector<int>::iterator, IteratorCategory::eContiguous>>);

	} // namespace __internals


	template <typename T, typename = void>
	struct std_iterator_converter {
		using type = T;
	};

	template <typename T>
	struct std_iterator_converter<T,
		std::enable_if_t<
			is_std_iterator_v<T>
		, void>
	> {
		using type = __internals::std_iterator_wrapper<T, iterator_traits<T>::CATEGORY>;
	};

	template <typename T>
	using std_iterator_converter_t = typename std_iterator_converter<T>::type;


	template <typename It>
	constexpr auto distance(const It &lhs, const It &rhs) noexcept -> typename iterator_traits<It>::ArithmeticType;

} // namespace flex

#include "flex/iterator.inl"
