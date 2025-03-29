#pragma once

#include <type_traits>

#include "flex/reflection/enums.hpp"


namespace flex {
	template <typename T>
	struct ErrorCode {
		static constexpr bool IS_ERROR_CODE {false};

		static constexpr auto isSuccess(T code) noexcept -> bool;
		static constexpr auto isFailure(T code) noexcept -> bool;
	};

	template <flex::enumeration T>
	struct ErrorCode<T> {
		static constexpr bool IS_ERROR_CODE {true};

		static constexpr auto isSuccess(T code) noexcept -> bool {
			return static_cast<std::underlying_type_t<T>> (code) == static_cast<std::underlying_type_t<T>> (0);
		}

		static constexpr auto isFailure(T code) noexcept -> bool {
			return !isSuccess(code);
		}
	};

	template <std::integral T>
	struct ErrorCode<T> {
		static constexpr bool IS_ERROR_CODE {true};

		static constexpr auto isSuccess(T code) noexcept -> bool {
			return code == static_cast<T> (0);
		}

		static constexpr auto isFailure(T code) noexcept -> bool {
			return !isSuccess(code);
		}
	};


	template <typename T>
	constexpr auto is_error_code_v = ErrorCode<T>::IS_ERROR_CODE;

} // namespace flex
