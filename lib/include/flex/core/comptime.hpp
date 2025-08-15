#pragma once

#include <string_view>


namespace flex {
	inline auto comptimeError([[maybe_unused]] std::string_view msg) noexcept -> void {}
	template <typename... Args>
	inline auto comptimeErrorWithTypes([[maybe_unused]] std::string_view msg) noexcept -> void {}
}
