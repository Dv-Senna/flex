#pragma once

#include "flex/core/typeTraits.hpp"
#include "flex/pipes/pipes.hpp"


namespace flex::pipes {
	template <typename Callback>
	class TransformPipe final {
		public:
		// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
			constexpr TransformPipe(Callback &&callback) noexcept : m_callback {std::forward<Callback> (callback)} {}
			TransformPipe(const TransformPipe&) = delete;
			auto operator=(const TransformPipe&) -> TransformPipe& = delete;
			TransformPipe(TransformPipe&&) = delete;
			auto operator=(TransformPipe&&) -> TransformPipe& = delete;
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
				return std::forward<Optional> (optional).transform(m_callback);
			};


		private:
			Callback m_callback;
	};

	constexpr flex::pipes::TemplatedPipeAdaptator<TransformPipe> transform {};

} // namespace flex::pipes
