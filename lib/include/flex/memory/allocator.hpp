#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <type_traits>

#include "flex/errorCode.hpp"
#include "flex/errorType.hpp"
#include "flex/typeTraits.hpp"


namespace flex::memory {
	template<class Alloc>
	concept std_allocator = requires(Alloc alloc, std::size_t n) {
		{*alloc.allocate(n)} -> std::same_as<std::add_lvalue_reference_t<typename Alloc::value_type>>;
		alloc.deallocate(alloc.allocate(n), n);
	} && std::copy_constructible<Alloc>
		&& std::equality_comparable<Alloc>;


	template <typename Alloc>
	concept allocator = requires(Alloc alloc, const Alloc constAlloc, std::size_t n) {
		typename Alloc::ValueType;
		{alloc.allocate(n)} -> flex::error_type;
		{*flex::error_type_traits<decltype(alloc.allocate(n))>::getValue(alloc.allocate(n))} -> std::same_as<std::add_lvalue_reference_t<typename Alloc::ValueType>>;
		alloc.deallocate(flex::error_type_traits<decltype(alloc.allocate(n))>::getValue(alloc.allocate(n)), n);
		{constAlloc.template rebind<typename Alloc::ValueType> ()} -> flex::error_type;

		noexcept(alloc.allocate(n)) == true;
		noexcept(alloc.deallocate(*alloc.allocate(n), n)) == true;
		noexcept(constAlloc.template rebind<typename Alloc::ValueType> ()) == true;
	} && std::copy_constructible<Alloc>
		&& std::equality_comparable<Alloc>
		&& std::default_initializable<Alloc>
		&& std::same_as<
			std::remove_cvref_t<decltype(flex::error_type_traits<
				decltype(std::declval<const Alloc> ().template rebind<typename Alloc::ValueType> ())
			>::getValue(std::declval<const Alloc> ().template rebind<typename Alloc::ValueType> ()))>,
			Alloc
		>;


	namespace __internals {
		template <typename Alloc>
		concept allocator_with_custom_pointer = allocator<Alloc> && requires() {
			typename Alloc::PointerType;
			typename Alloc::ConstPointerType;
		} && std::convertible_to<typename Alloc::PointerType, typename Alloc::ConstPointerType>;

		template <typename Alloc>
		concept allocator_with_custom_void_pointer = allocator_with_custom_pointer<Alloc> && requires() {
			typename Alloc::VoidPointerType;
			typename Alloc::ConstVoidPointerType;
		} && std::convertible_to<typename Alloc::VoidPointerType, typename Alloc::ConstVoidPointerType>
			&& std::convertible_to<typename Alloc::PointerType, typename Alloc::VoidPointerType>
			&& std::convertible_to<typename Alloc::PointerType, typename Alloc::ConstVoidPointerType>
			&& std::convertible_to<typename Alloc::ConstPointerType, typename Alloc::ConstVoidPointerType>;

		template <typename Alloc>
		concept allocator_with_custom_size_type = allocator<Alloc> && requires() {
			typename Alloc::SizeType;
		} && std::unsigned_integral<typename Alloc::SizeType>;

		template <typename Alloc>
		concept allocator_with_custom_difference_type = allocator<Alloc> && requires() {
			typename Alloc::DifferenceType;
		} && std::signed_integral<typename Alloc::DifferenceType>;

		template <typename Alloc, typename Rebind>
		concept allocator_with_custom_rebind_type = allocator<Alloc> && requires() {
			typename Alloc::template RebindType<Rebind>;
		};


		template <typename Alloc, typename Rebind>
		struct rebind_allocator;

		template <template <typename, typename...> typename Alloc, typename T, typename ...Args, typename Rebind>
		struct rebind_allocator<Alloc<T, Args...>, Rebind> {
			using type = Alloc<Rebind, Args...>;
		};


		template <typename Alloc>
		concept allocator_with_custom_destroy = allocator<Alloc> && requires(Alloc alloc, std::size_t n) {
			alloc.destroy(flex::error_type_traits<decltype(alloc.allocate(n))>::getValue(alloc.allocate(n)));
			noexcept(alloc.destroy(flex::error_type_traits<decltype(alloc.allocate(n))>::getValue(alloc.allocate(n)))) == true;
		};

		template <typename Alloc, typename ...Args>
		concept allocator_with_custom_construct = allocator<Alloc> && requires(Alloc alloc, std::size_t n, Args ...args) {
			alloc.construct(flex::error_type_traits<decltype(alloc.allocate(n))>::getValue(alloc.allocate(n)), args...);
			noexcept(alloc.construct(flex::error_type_traits<decltype(alloc.allocate(n))>::getValue(alloc.allocate(n)), args...)) == true;
		} && allocator_with_custom_destroy<Alloc>;

		template <typename Alloc>
		concept allocator_with_custom_always_equal = allocator<Alloc> && requires() {
			{Alloc::IS_ALWAYS_EQUAL} -> std::same_as<bool>;
		};

		template <typename Alloc>
		concept allocator_with_custom_copy_on_container_copy = allocator<Alloc> && requires(const Alloc alloc) {
			alloc.copyOnContainerCopy();
		};


		template <allocator Alloc>
		struct allocator_pointer : flex::type_constant<std::add_pointer_t<typename Alloc::ValueType>> {};

		template <allocator_with_custom_pointer Alloc>
		struct allocator_pointer<Alloc> : flex::type_constant<typename Alloc::PointerType> {};


		template <allocator Alloc>
		struct allocator_const_pointer : flex::type_constant<std::add_pointer_t<std::add_const_t<typename Alloc::ValueType>>> {};

		template <allocator_with_custom_pointer Alloc>
		struct allocator_const_pointer<Alloc> : flex::type_constant<typename Alloc::ConstPointerType> {};


		template <allocator Alloc>
		struct allocator_void_pointer : flex::type_constant<void*> {};

		template <allocator_with_custom_void_pointer Alloc>
		struct allocator_void_pointer<Alloc> : flex::type_constant<typename Alloc::VoidPointerType> {};


		template <allocator Alloc>
		struct allocator_const_void_pointer : flex::type_constant<const void*> {};

		template <allocator_with_custom_void_pointer Alloc>
		struct allocator_const_void_pointer<Alloc> : flex::type_constant<typename Alloc::ConstVoidPointerType> {};
	

		template <allocator Alloc>
		struct allocator_size : flex::type_constant<std::size_t> {};

		template <allocator_with_custom_size_type T>
		struct allocator_size<T> : flex::type_constant<typename T::SizeType> {};


		template <allocator Alloc>
		struct allocator_difference : flex::type_constant<std::ptrdiff_t> {};

		template <allocator_with_custom_difference_type T>
		struct allocator_difference<T> : flex::type_constant<typename T::DifferenceType> {};


		template <allocator Alloc, typename Rebind, typename = void>
		struct allocator_rebind : rebind_allocator<Alloc, Rebind> {};

		template <typename Rebind, allocator_with_custom_rebind_type<Rebind> Alloc>
		struct allocator_rebind<Alloc, Rebind, void> : flex::type_constant<typename Alloc::template RebindType<Rebind>> {};

	} // namespace __internals


	template <allocator T>
	struct allocator_traits {
		using Type = T;
		using ValueType = T::ValueType;
		using PointerType = typename __internals::allocator_pointer<T>::type;
		using ConstPointerType = typename __internals::allocator_const_pointer<T>::type;
		using VoidPointerType = typename __internals::allocator_void_pointer<T>::type;
		using ConstVoidPointerType = typename __internals::allocator_const_void_pointer<T>::type;
		using SizeType = typename __internals::allocator_size<T>::type;
		using DifferenceType = typename __internals::allocator_difference<T>::type;

		template <typename Rebind>
		using RebindType = typename __internals::allocator_rebind<T, Rebind>::type;

		template <typename Rebind>
		using rebind_traits = allocator_traits<RebindType<Rebind>>;

		static constexpr bool IS_ALWAYS_EQUAL = __internals::allocator_with_custom_always_equal<T>
			? T::IS_ALWAYS_EQUAL
			: std::is_empty_v<T>;


		[[nodiscard]]
		static constexpr auto allocate(T &instance, std::size_t n) noexcept {return instance.allocate(n);}
		static constexpr auto deallocate(T &instance, const PointerType &ptr, std::size_t n) noexcept {return instance.deallocate(ptr, n);}

		template <typename ...Args>
		static constexpr auto construct(T &instance, const PointerType &ptr, Args &&...args) noexcept {
			if constexpr (__internals::allocator_with_custom_construct<T, Args...>)
				return instance.construct(ptr, std::forward<Args> (args)...);
			else
				std::construct_at(std::to_address(ptr), std::forward<Args> (args)...);
		}

		static constexpr auto destroy(T &instance, const PointerType &ptr) noexcept {
			if constexpr (__internals::allocator_with_custom_destroy<T>)
				return instance.destroy(ptr);
			else
				std::destroy_at(std::to_address(ptr));
		}

		template <typename Rebind>
		[[nodiscard]]
		static constexpr auto rebind(const T &instance) noexcept {return instance.template rebind<Rebind> ();}

		[[nodiscard]]
		static constexpr auto copyOnContainerCopy(const T &instance) noexcept {
			if constexpr (__internals::allocator_with_custom_copy_on_container_copy<T>)
				return instance.copyOnContainerCopy();
			else
				return instance;
		}
	};


	template <allocator T>
	using allocator_value_t = typename allocator_traits<T>::ValueType;

	template <allocator T>
	using allocator_pointer_t = typename allocator_traits<T>::PointerType;

	template <allocator T>
	using allocator_const_pointer_t = typename allocator_traits<T>::ConstPointerType;

	template <allocator T>
	using allocator_void_pointer_t = typename allocator_traits<T>::VoidPointerType;

	template <allocator T>
	using allocator_const_void_pointer_t = typename allocator_traits<T>::ConstVoidPointerType;

	template <allocator T>
	using allocator_size_t = typename allocator_traits<T>::SizeType;

	template <allocator T>
	using allocator_difference_t = typename allocator_traits<T>::DifferenceType;

	template <allocator T, typename U>
	using allocator_rebind_t = typename allocator_traits<T>::template RebindType<U>;

	template <allocator T, typename ...Args>
	constexpr bool is_allocator_construct_failable_v = flex::error_code<decltype(allocator_traits<T>::construct(
		std::declval<T&> (),
		std::declval<allocator_pointer_t<T>> (),
		std::declval<Args> ()...
	))>;

	template <allocator T>
	constexpr bool is_allocator_destroy_failable_v = flex::error_code<decltype(allocator_traits<T>::destroy(
		std::declval<T&> (),
		std::declval<allocator_pointer_t<T>> ()
	))>;

	template <allocator T>
	constexpr auto is_allocator_always_equal_v = allocator_traits<T>::IS_ALWAYS_EQUAL;


	template <typename T>
	concept stateful_allocator = allocator<T> && !is_allocator_always_equal_v<T>;

	template <typename T>
	concept stateless_allocator = allocator<T> && is_allocator_always_equal_v<T>;


	enum class AllocatorErrorCode {
		eSuccess = 0,
		eFailure,
		eAllocationFailure,
		eDeallocationFailure,

		eStackSharedStateAllocationFailure,
		eStackSharedAllocatorRebindFailure,
		eStackBlockAllocationFailure,
		eStackSharedStateCreationFailure,
		eStackUnifiedAllocatorRebindFailure,
		eStackNotInitialized,
		eStackTooSmall,
		eStackInvalidPtr,

		ePoolSharedStateAllocationFailure,
		ePoolSharedAllocatorRebindFailure,
		ePoolBlockAllocationFailure,
		ePoolSharedStateCreationFailure,
		ePoolNotInitialized,
		ePoolIsFull,
		ePoolInvalidPtr,
		ePoolBlockAllocatorRebindFailure,
		ePoolAllocationSizeNotOne,
	};

	template <std_allocator T>
	class StandardAllocatorWrapper final {
		template <std_allocator U>
		friend class StandardAllocatorWrapper;

		public:
			using ValueType = std::allocator_traits<T>::value_type;
			using PointerType = std::allocator_traits<T>::pointer;
			using ConstPointerType = std::allocator_traits<T>::const_pointer;
			using VoidPointerType = std::allocator_traits<T>::void_pointer;
			using ConstVoidPointerType = std::allocator_traits<T>::const_void_pointer;
			using SizeType = std::allocator_traits<T>::size_type;
			using DifferenceType = std::allocator_traits<T>::difference_type;

			template <typename Rebind>
			using RebindType = std::allocator_traits<T>::template rebind_alloc<Rebind>;

			static constexpr bool IS_ALWAYS_EQUAL = std::allocator_traits<T>::is_always_equal;
			static constexpr bool STORE_ALLOCATOR = !IS_ALWAYS_EQUAL;

			constexpr StandardAllocatorWrapper() noexcept = default;
			constexpr ~StandardAllocatorWrapper() = default;
			constexpr StandardAllocatorWrapper(const StandardAllocatorWrapper<T>&) noexcept = default;
			constexpr auto operator=(const StandardAllocatorWrapper<T>&) noexcept -> StandardAllocatorWrapper<T>& = default;
			constexpr StandardAllocatorWrapper(StandardAllocatorWrapper<T>&&) noexcept = default;
			constexpr auto operator=(StandardAllocatorWrapper<T>&&) noexcept -> StandardAllocatorWrapper<T>& = default;

			constexpr StandardAllocatorWrapper(const T &allocator) noexcept requires (STORE_ALLOCATOR) :
				m_allocator {allocator}
			{}

			constexpr StandardAllocatorWrapper(T &&allocator) noexcept requires (STORE_ALLOCATOR) :
				m_allocator {std::move(allocator)}
			{}

			template <typename ...Args>
			requires (sizeof...(Args) != 0)
			constexpr StandardAllocatorWrapper(Args &&...args) noexcept requires (STORE_ALLOCATOR) :
				m_allocator {std::forward<Args> (args)...}
			{}


			[[nodiscard]]
			constexpr auto allocate(std::size_t n) noexcept -> std::optional<PointerType> {
				try {
					return std::allocator_traits<T>::allocate(this->m_getAllocator(), n);
				}
				catch (...) {
					return std::nullopt;
				}
			};

			constexpr auto deallocate(const PointerType &ptr, std::size_t n) noexcept {
				if constexpr (noexcept(std::declval<T> ().deallocate(ptr, n))) {
					std::allocator_traits<T>::deallocate(this->m_getAllocator(), ptr, n);
				}
				else {
					try {
						std::allocator_traits<T>::deallocate(this->m_getAllocator(), ptr, n);
						return AllocatorErrorCode::eSuccess;
					}
					catch (...) {
						return AllocatorErrorCode::eFailure;
					}
				}
			};

			template <typename ...Args>
			[[nodiscard]]
			constexpr auto construct(const PointerType &ptr, Args &&...args) noexcept -> AllocatorErrorCode {
				try {
					std::allocator_traits<T>::construct(this->m_getAllocator(), ptr, std::forward<Args> (args)...);
					return AllocatorErrorCode::eSuccess;
				}
				catch (...) {
					return AllocatorErrorCode::eFailure;
				}
			};

			constexpr auto destroy(const PointerType &ptr) noexcept -> AllocatorErrorCode {
				try {
					std::allocator_traits<T>::destroy(this->m_getAllocator(), ptr);
					return AllocatorErrorCode::eSuccess;
				}
				catch (...) {
					return AllocatorErrorCode::eFailure;
				}
			};


			template <typename Rebind>
			[[nodiscard]]
			constexpr auto rebind() const noexcept -> std::optional<RebindType<Rebind>> {
				try {
					return RebindType<Rebind> {*this};
				}
				catch (...) {
					return std::nullopt;
				}
			}


			[[nodiscard]]
			constexpr auto copyOnContainerCopy() const noexcept -> std::optional<StandardAllocatorWrapper<T>> {
				try {
					return StandardAllocatorWrapper<T> {std::allocator_traits<T>::select_on_container_copy_construction(this->m_getAllocator())};
				}
				catch (...) {
					return std::nullopt;
				}
			};


		private:
			constexpr auto m_getAllocator() noexcept -> T& requires (STORE_ALLOCATOR) {return m_allocator;}
			constexpr auto m_getAllocator() const noexcept -> const T& requires (STORE_ALLOCATOR) {return m_allocator;}
			constexpr auto m_getAllocator() const noexcept -> T requires (!STORE_ALLOCATOR) {return T{};}

			[[no_unique_address]]
			std::conditional_t<STORE_ALLOCATOR, T, flex::Empty> m_allocator;
	};


	template <typename T>
	class Allocator final {
		public:
			using ValueType = T;
			using PointerType = std::add_pointer_t<T>;
			using ConstPointerType = std::add_pointer_t<std::add_const_t<T>>;

			template <typename Rebind>
			using RebindType = Allocator<Rebind>;

			static constexpr bool IS_ALWAYS_EQUAL {true};

			constexpr Allocator() noexcept = default;
			constexpr Allocator(const Allocator<T>&) noexcept = default;
			constexpr auto operator=(const Allocator<T>&) noexcept -> Allocator& = default;
			constexpr Allocator(Allocator<T>&&) noexcept = default;
			constexpr auto operator=(Allocator<T>&&) noexcept -> Allocator& = default;

			constexpr auto operator==(const Allocator<T>&) const noexcept -> bool {return true;}

			[[nodiscard]]
			auto allocate(std::size_t n) const noexcept -> std::optional<PointerType> {
				auto ptr {reinterpret_cast<PointerType> (std::aligned_alloc(alignof(T), sizeof(ValueType) * n))};
				if (ptr == nullptr)
					return std::nullopt;
				return ptr;
			}

			auto deallocate(PointerType ptr, std::size_t) const noexcept -> void {
				std::free(ptr);
			}

			template <typename Rebind>
			[[nodiscard]]
			constexpr auto rebind() const noexcept -> std::optional<RebindType<Rebind>> {
				return RebindType<Rebind> {};
			}
	};

	static_assert(allocator<Allocator<int>>);
	static_assert(__internals::allocator_with_custom_rebind_type<Allocator<int>, float>);
	static_assert(std::is_same_v<allocator_rebind_t<Allocator<int>, float>, Allocator<float>>);

} // namespace flex::memory
