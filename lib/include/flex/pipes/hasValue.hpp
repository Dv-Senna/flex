#pragma once

#include "flex/core/typeTraits.hpp"
#include "flex/pipes/pipes.hpp"


namespace flex::pipes {
	class HasValuePipe final {
		public:
			constexpr HasValuePipe() noexcept = default;
			HasValuePipe(const HasValuePipe&) = delete;
			auto operator=(const HasValuePipe&) -> HasValuePipe& = delete;
			HasValuePipe(HasValuePipe&&) = delete;
			auto operator=(HasValuePipe&&) -> HasValuePipe& = delete;
			constexpr ~HasValuePipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				return std::forward<Optional> (optional).has_value();
			}
	};

	constexpr flex::pipes::PipeAdaptator<HasValuePipe> has_value {};

} // namespace flex::pipes
