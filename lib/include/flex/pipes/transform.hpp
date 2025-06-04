#pragma once

#include "flex/pipes/pipes.hpp"
#include "flex/typeTraits.hpp"


namespace flex::pipes {
	template <typename Callback>
	class TransformPipe : public flex::pipes::BasicPipeObject<TransformPipe<Callback>> {
		public:
			constexpr TransformPipe(Callback &&callback) noexcept : m_callback {std::forward<Callback> (callback)} {}
			constexpr ~TransformPipe() = default;


			template <typename EntryType>
			constexpr auto operator()(EntryType &&entry) noexcept {
				return m_callback(std::forward<EntryType> (entry));
			}

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			constexpr auto operator()(Optional &&optional) noexcept {
				using Result = decltype(m_callback(*optional));
				if (!optional)
					return std::optional<Result> {std::nullopt};
				return std::optional<Result> {m_callback(*optional)};
			};


		private:
			Callback m_callback;
	};

	constexpr flex::pipes::TemplatedPipeAdaptator<TransformPipe> transform {};

} // namespace flex::pipes
