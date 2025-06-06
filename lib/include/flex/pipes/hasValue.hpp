#pragma once

#include "flex/pipes/pipes.hpp"
#include "flex/typeTraits.hpp"


namespace flex::pipes {
	class HasValuePipe final {
		public:
			constexpr HasValuePipe() noexcept = default;
			constexpr ~HasValuePipe() = default;

			template <typename Optional>
			requires flex::optional<std::remove_cvref_t<Optional>>
			[[nodiscard]]
			constexpr auto operator()(Optional &&optional) noexcept {
				return optional.has_value();
			}
	};

	constexpr flex::pipes::PipeAdaptator<HasValuePipe> has_value {};

} // namespace flex::pipes
