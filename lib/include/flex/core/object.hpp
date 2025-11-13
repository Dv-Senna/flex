#pragma once

#include <concepts>
#include <type_traits>

#include "flex/core/typeTraits.hpp"


namespace flex::core {
	/**
	 * @defgroup Object Named Requirements: *Object*
	 * @brief Object model of flex
	 *
	 * ### Requirements
	 * Given
	 *     - `T` some *Object* type
	 *
	 * We have that
	 *     - `T` must be *non-copyable* and *default-movable* (assignation and construction)
	 *     - an instance of the type `T` must be only buildable by a factory static method of `T`, with name `create`
	 *       for example. One exception is default construction
	 *     - Cloning of an instance must be done through the `clone` method (see clone_traits)
	 * */



	template <typename T>
	concept object = std::is_class_v<std::remove_cvref_t<T>>
		&& !std::copyable<std::remove_cvref_t<T>>
		&& std::movable<std::remove_cvref_t<T>>;


	template <typename T>
	concept nofail_clonable = object<T> && requires (const std::remove_cvref_t<T> cv) {
		{cv.clone()} -> std::same_as<std::remove_cvref_t<T>>;
	};

	template <typename T>
	concept fail_clonable = object<T> && requires (const std::remove_cvref_t<T> cv) {
		{cv.clone()} -> flex::outcome;
	};

	template <typename T>
	concept clonable = nofail_clonable<T> || fail_clonable<T>;
}
