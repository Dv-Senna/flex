#pragma once

#include "flex/core/typeTraits.hpp"
#include "flex/pipes/pipes.hpp"


namespace flex::pipes {
	template <typename T>
	class ValueOrPipe final {
		public:
		// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
			constexpr ValueOrPipe(T &&value) noexcept : m_value {std::forward<T> (value)} {}
			ValueOrPipe(const ValueOrPipe&) = delete;
			auto operator=(const ValueOrPipe&) -> ValueOrPipe& = delete;
			ValueOrPipe(ValueOrPipe&&) = delete;
			auto operator=(ValueOrPipe&&) -> ValueOrPipe& = delete;
			constexpr ~ValueOrPipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				return std::forward<Optional> (optional).value_or(m_value);
			}

		private:
			T m_value;
	};

	template <typename T>
	ValueOrPipe(T &&value) -> ValueOrPipe<T>;


	constexpr flex::pipes::TemplatedPipeAdaptator<ValueOrPipe> value_or {};

} // namespace flex::pipes
