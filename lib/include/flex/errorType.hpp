#pragma once

#include <expected>
#include <optional>


namespace flex {
	namespace __internals {
		template <typename T>
		concept autogen_error_type = requires(T v, const T cv) {
			typename T::ValueType;
			{*v} -> std::same_as<std::add_lvalue_reference_t<typename T::ValueType>>;
			{*cv} -> std::same_as<std::add_lvalue_reference_t<std::add_const_t<typename T::ValueType>>>;
			{v.operator->()} -> std::same_as<std::add_pointer_t<typename T::ValueType>>;
			{cv.operator->()} -> std::same_as<std::add_pointer_t<std::add_const_t<typename T::ValueType>>>;
			{!cv} -> std::same_as<bool>;
		} && std::movable<T>;


		template <typename T>
		concept autogen_error_type_with_error = requires(T v, const T cv) {
			typename T::ErrorType;
			{v.getError()} -> std::same_as<std::add_lvalue_reference_t<typename T::ErrorType>>;
			{cv.getError()} -> std::same_as<std::add_lvalue_reference_t<std::add_const_t<typename T::ErrorType>>>;
		} && autogen_error_type<T>;

	} // namespace __internals


	template <typename T>
	struct error_type_traits {
		static constexpr bool IS_ERROR_TYPE {false};
	};


	template <typename T>
	struct error_type_traits<std::optional<T>> {
		static constexpr bool IS_ERROR_TYPE {true};
		using Type = std::optional<T>;
		using ValueType = T;
		using ErrorType = void;
		
		static constexpr auto hasValue(const Type &instance) noexcept -> bool {return !!instance;}
		static constexpr auto getValue(Type &instance) noexcept -> ValueType& {return *instance;}
		static constexpr auto getValue(const Type &instance) noexcept -> const ValueType& {return *instance;}

		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(Type &&instance, U &&defaultValue) noexcept -> ValueType {return instance.value_or(std::forward<U> (defaultValue));}
		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(const Type &instance, U &&defaultValue) noexcept -> ValueType {return instance.value_or(std::forward<U> (defaultValue));}
	};


	template <typename T, typename E>
	struct error_type_traits<std::expected<T, E>> {
		static constexpr bool IS_ERROR_TYPE {true};
		using Type = std::expected<T, E>;
		using ValueType = T;
		using ErrorType = E;

		static constexpr auto hasValue(const Type &instance) noexcept -> bool {return !!instance;}
 		static constexpr auto getValue(Type &instance) noexcept -> ValueType& {return *instance;}
		static constexpr auto getValue(const Type &instance) noexcept -> const ValueType& {return *instance;}

		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(Type &&instance, U &&defaultValue) noexcept -> ValueType {return instance.value_or(std::forward<U> (defaultValue));}
		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(const Type &instance, U &&defaultValue) noexcept -> ValueType {return instance.value_or(std::forward<U> (defaultValue));}

		static constexpr auto getError(Type &instance) noexcept -> ValueType& {return instance.error();}
		static constexpr auto getError(const Type &instance) noexcept -> const ValueType& {return instance.error();}

		template <typename G = std::remove_cvref_t<T>>
		static constexpr auto getErrorOr(Type &&instance, G &&defaultValue) noexcept -> ErrorType {return instance.error_or(std::forward<G> (defaultValue));}
		template <typename G = std::remove_cvref_t<T>>
		static constexpr auto getErrorOr(const Type &instance, G &&defaultValue) noexcept -> ErrorType {return instance.error_or(std::forward<G> (defaultValue));}
	};


	template <__internals::autogen_error_type T>
	struct error_type_traits<T> {
		static constexpr bool IS_ERROR_TYPE {true};
		using Type = T;
		using ValueType = T::ValueType;
		using ErrorType = void;

		static constexpr auto hasValue(const Type &instance) noexcept -> bool {return !!instance;}
		static constexpr auto getValue(Type &instance) noexcept -> ValueType& {return *instance;}
		static constexpr auto getValue(const Type &instance) noexcept -> const ValueType& {return *instance;}

		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(Type &&instance, U &&defaultValue) noexcept -> ValueType {return !instance ? defaultValue : *instance;}
		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(const Type &instance, U &&defaultValue) noexcept -> ValueType {return !instance ? defaultValue : *instance;}
	};


	template <__internals::autogen_error_type_with_error T>
	struct error_type_traits<T> {
		static constexpr bool IS_ERROR_TYPE {true};
		using Type = T;
		using ValueType = T::ValueType;
		using ErrorType = void;

		static constexpr auto hasValue(const Type &instance) noexcept -> bool {return !!instance;}
		static constexpr auto getValue(Type &instance) noexcept -> ValueType& {return *instance;}
		static constexpr auto getValue(const Type &instance) noexcept -> const ValueType& {return *instance;}

		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(Type &&instance, U &&defaultValue) noexcept -> ValueType {return !instance ? defaultValue : *instance;}
		template <typename U = std::remove_cvref_t<T>>
		static constexpr auto getValueOr(const Type &instance, U &&defaultValue) noexcept -> ValueType {return !instance ? defaultValue : *instance;}

		static constexpr auto getError(Type &instance) noexcept -> ValueType& {return instance.error();}
		static constexpr auto getError(const Type &instance) noexcept -> const ValueType& {return instance.error();}

		template <typename G = std::remove_cvref_t<T>>
		static constexpr auto getErrorOr(Type &&instance, G &&defaultValue) noexcept -> ErrorType {return !instance ? instance.getError() : defaultValue;}
		template <typename G = std::remove_cvref_t<T>>
		static constexpr auto getErrorOr(const Type &instance, G &&defaultValue) noexcept -> ErrorType {return !instance ? instance.getError() : defaultValue;}
	};


	template <typename T>
	constexpr auto is_error_type_v = error_type_traits<T>::IS_ERROR_TYPE;

	template <typename T>
	concept error_type = is_error_type_v<T>;


	template <error_type T>
	constexpr auto is_error_type_with_error_v = !std::is_void_v<typename error_type_traits<T>::ErrorType>;

	template <typename T>
	concept error_type_with_error = error_type<T> && is_error_type_with_error_v<T>;

	template <error_type T>
	constexpr auto is_error_type_without_error_v = std::is_void_v<typename error_type_traits<T>::ErrorType>;

	template <typename T>
	concept error_type_without_error = error_type<T> && is_error_type_without_error_v<T>;


	template <error_type T>
	using error_type_value_t = typename error_type_traits<T>::ValueType;

	template <error_type_with_error T>
	using error_type_error_t = typename error_type_traits<T>::ErrorType;

} // namespace flex
