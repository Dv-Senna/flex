#pragma once

#include <any>
#include <tuple>

#include "flex/pipes/pipes.hpp"
#include "flex/typeTraits.hpp"


namespace flex::pipes {
	template <typename T>
	class StaticCastToPipe final {
		public:
			constexpr StaticCastToPipe() noexcept = default;
			constexpr ~StaticCastToPipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!optional)
					return std::nullopt;
				return static_cast<T> (*optional);
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<StaticCastToPipe<T>> static_cast_to {};


	template <typename T>
	class ReinterpretCastToPipe final {
		public:
			constexpr ReinterpretCastToPipe() noexcept = default;
			constexpr ~ReinterpretCastToPipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!optional)
					return std::nullopt;
				return reinterpret_cast<T> (*optional);
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<ReinterpretCastToPipe<T>> reinterpret_cast_to {};


	template <typename T>
	class DynamicCastToPipe final {
		public:
			constexpr DynamicCastToPipe() noexcept = default;
			constexpr ~DynamicCastToPipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!optional)
					return std::nullopt;
				return dynamic_cast<T> (*optional);
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<DynamicCastToPipe<T>> dynamic_cast_to {};


	template <typename T>
	class AnyCastToPipe final {
		public:
			constexpr AnyCastToPipe() noexcept = default;
			constexpr ~AnyCastToPipe() = default;

			template <typename Any>
			requires std::same_as<std::remove_cvref_t<Any>, std::any>
			[[nodiscard]]
			constexpr auto operator()(Any &&any) noexcept -> std::optional<T> {
				if (!any.has_value())
					return std::nullopt;
				if (any.type() != typeid(T))
					return std::nullopt;
				return std::any_cast<T> (any);
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
				&& std::same_as<std::remove_cvref_t<typename std::remove_cvref_t<Optional>::value_type>, std::any>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!optional)
					return std::nullopt;
				return (*this)(*optional);
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<AnyCastToPipe<T>> any_cast_to {};



	template <flex::no_cv_reference T>
	class ConstructToPipe final {
		public:
			constexpr ConstructToPipe() noexcept = default;
			constexpr ~ConstructToPipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!optional)
					return std::nullopt;
				return T{*optional};
			}
	};

	template <flex::no_cv_reference T>
	constexpr flex::pipes::PipeAdaptator<ConstructToPipe<T>> construct_to {};

} // namespace flex::pipes
