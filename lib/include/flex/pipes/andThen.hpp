#pragma once

#include "flex/core/typeTraits.hpp"
#include "flex/pipes/pipes.hpp"


namespace flex::pipes {
	template <typename Callback>
	class AndThenPipe final {
		public:
		// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
			constexpr AndThenPipe(Callback &&callback) noexcept : m_callback {std::forward<Callback> (callback)} {}
			AndThenPipe(const AndThenPipe&) = delete;
			auto operator=(const AndThenPipe&) -> AndThenPipe& = delete;
			AndThenPipe(AndThenPipe&&) = delete;
			auto operator=(AndThenPipe&&) -> AndThenPipe& = delete;
			constexpr ~AndThenPipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				return std::forward<Optional> (optional).and_then(m_callback);
			}

		private:
			Callback m_callback;
	};


	constexpr flex::pipes::TemplatedPipeAdaptator<AndThenPipe> and_then {};

} // namespace flex::pipes
