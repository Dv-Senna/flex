#pragma once

#include "flex/pipes/pipes.hpp"
#include "flex/typeTraits.hpp"


namespace flex::pipes {
	template <typename Callback>
	class AndThenPipe final {
		public:
			constexpr AndThenPipe(Callback &&callback) noexcept : m_callback {std::forward<Callback> (callback)} {}
			constexpr ~AndThenPipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				return optional.and_then(m_callback);
			}

		private:
			Callback m_callback;
	};


	constexpr flex::pipes::TemplatedPipeAdaptator<AndThenPipe> and_then {};

} // namespace flex::pipes
