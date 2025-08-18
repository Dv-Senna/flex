#pragma once

#include <any>

#include "flex/core/reference.hpp"
#include "flex/core/typeTraits.hpp"
#include "flex/pipes/pipes.hpp"


namespace flex::pipes {
	template <typename T>
	class StaticCastToPipe final {
		public:
			constexpr StaticCastToPipe() noexcept = default;
			StaticCastToPipe(const StaticCastToPipe&) = delete;
			auto operator=(const StaticCastToPipe&) -> StaticCastToPipe& = delete;
			StaticCastToPipe(StaticCastToPipe&&) = delete;
			auto operator=(StaticCastToPipe&&) -> StaticCastToPipe& = delete;
			constexpr ~StaticCastToPipe() = default;

			template <typename U>
			[[nodiscard]]
			constexpr auto operator()(U &&value) noexcept {
				return static_cast<T> (std::forward<U> (value));
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!std::forward<Optional> (optional))
					return std::nullopt;
				return (*this)(*std::forward<Optional> (optional));
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<StaticCastToPipe<T>> static_cast_to {};


	template <typename T>
	class ReinterpretCastToPipe final {
		public:
			constexpr ReinterpretCastToPipe() noexcept = default;
			ReinterpretCastToPipe(const ReinterpretCastToPipe&) = delete;
			auto operator=(const ReinterpretCastToPipe&) -> ReinterpretCastToPipe& = delete;
			ReinterpretCastToPipe(ReinterpretCastToPipe&&) = delete;
			auto operator=(ReinterpretCastToPipe&&) -> ReinterpretCastToPipe& = delete;
			constexpr ~ReinterpretCastToPipe() = default;

			template <typename U>
			[[nodiscard]]
			constexpr auto operator()(U &&value) noexcept {
			// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
				return reinterpret_cast<T> (std::forward<U> (value));
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!std::forward<Optional> (optional))
					return std::nullopt;
				return (*this)(*std::forward<Optional> (optional));
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<ReinterpretCastToPipe<T>> reinterpret_cast_to {};


	template <typename T>
	class DynamicCastToPipe final {
		using ReturnType = std::conditional_t<flex::reference<T>,
			flex::Reference<std::remove_reference_t<T>>,
			T
		>;
		public:
			constexpr DynamicCastToPipe() noexcept = default;
			DynamicCastToPipe(const DynamicCastToPipe&) = delete;
			auto operator=(const DynamicCastToPipe&) -> DynamicCastToPipe& = delete;
			DynamicCastToPipe(DynamicCastToPipe&&) = delete;
			auto operator=(DynamicCastToPipe&&) -> DynamicCastToPipe& = delete;
			constexpr ~DynamicCastToPipe() = default;

			template <typename U>
			[[nodiscard]]
			constexpr auto operator()(U &&value) noexcept -> std::optional<ReturnType> {
				using Pointerify = std::conditional_t<flex::reference<T>,
					std::add_pointer_t<std::remove_reference_t<T>>,
					T
				>;
				Pointerify res {nullptr};
				if constexpr (flex::reference<T>)
					res = dynamic_cast<Pointerify> (&std::forward<U> (value));
				else
					res = dynamic_cast<T> (std::forward<U> (value));

				if (res == nullptr)
					return std::nullopt;

				if constexpr (flex::reference<T>)
					return ReturnType{*res};
				else
					return res;
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<ReturnType> {
				if (!std::forward<Optional> (optional))
					return std::nullopt;
				return (*this)(*std::forward<Optional> (optional));
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<DynamicCastToPipe<T>> dynamic_cast_to {};


	template <typename T>
	class AnyCastToPipe final {
		public:
			constexpr AnyCastToPipe() noexcept = default;
			AnyCastToPipe(const AnyCastToPipe&) = delete;
			auto operator=(const AnyCastToPipe&) -> AnyCastToPipe& = delete;
			AnyCastToPipe(AnyCastToPipe&&) = delete;
			auto operator=(AnyCastToPipe&&) -> AnyCastToPipe& = delete;
			constexpr ~AnyCastToPipe() = default;

			template <typename Any>
			requires std::same_as<std::remove_cvref_t<Any>, std::any>
			[[nodiscard]]
			constexpr auto operator()(Any &&any) noexcept -> std::optional<T> {
				if (!std::forward<Any> (any).has_value())
					return std::nullopt;
				if (std::forward<Any> (any).type() != typeid(T))
					return std::nullopt;
				return std::any_cast<T> (std::forward<Any> (any));
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
				&& std::same_as<std::remove_cvref_t<typename std::remove_cvref_t<Optional>::value_type>, std::any>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!std::forward<Optional> (optional))
					return std::nullopt;
				return (*this)(*std::forward<Optional> (optional));
			}
	};

	template <typename T>
	constexpr flex::pipes::PipeAdaptator<AnyCastToPipe<T>> any_cast_to {};



	template <flex::no_cv_reference T>
	class ConstructToPipe final {
		public:
			constexpr ConstructToPipe() noexcept = default;
			ConstructToPipe(const ConstructToPipe&) = delete;
			auto operator=(const ConstructToPipe&) -> ConstructToPipe& = delete;
			ConstructToPipe(ConstructToPipe&&) = delete;
			auto operator=(ConstructToPipe&&) -> ConstructToPipe& = delete;
			constexpr ~ConstructToPipe() = default;

			template <typename U>
			[[nodiscard]]
			constexpr auto operator()(U &&value) noexcept {
				return T{std::forward<U> (value)};
			};

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept -> std::optional<T> {
				if (!std::forward<Optional> (optional))
					return std::nullopt;
				return (*this)(*std::forward<Optional> (optional));
			}
	};

	template <flex::no_cv_reference T>
	constexpr flex::pipes::PipeAdaptator<ConstructToPipe<T>> construct_to {};

}
