#pragma once

#include "flex/pipes/pipes.hpp"
#include "flex/typeTraits.hpp"


namespace flex::pipes {
	template <typename Callback>
	class TransformPipe final {
		public:
			constexpr TransformPipe(Callback &&callback) noexcept : m_callback {std::forward<Callback> (callback)} {}
			constexpr ~TransformPipe() = default;


			template <typename EntryType>
			[[nodiscard]]
			constexpr auto operator()(EntryType &&entry) noexcept {
				return m_callback(std::forward<EntryType> (entry));
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				return optional.transform(m_callback);
			};


		private:
			Callback m_callback;
	};

	constexpr flex::pipes::TemplatedPipeAdaptator<TransformPipe> transform {};

} // namespace flex::pipes
