#pragma once

#include <type_traits>

#include "flex/reflection/enums.hpp"


namespace flex {
	template <typename T>
	struct error_code_traits {
		static constexpr bool IS_ERROR_CODE {false};

		static constexpr auto isSuccess(T code) noexcept -> bool;
		static constexpr auto isFailure(T code) noexcept -> bool;
	};

	template <>
	struct error_code_traits<void> {
		static constexpr bool IS_ERROR_CODE {false};
	};

	template <flex::enumeration T>
	struct error_code_traits<T> {
		static constexpr bool IS_ERROR_CODE {true};

		static constexpr auto isSuccess(T code) noexcept -> bool {
			return static_cast<std::underlying_type_t<T>> (code) == static_cast<std::underlying_type_t<T>> (0);
		}

		static constexpr auto isFailure(T code) noexcept -> bool {
			return !isSuccess(code);
		}
	};

	template <std::integral T>
	struct error_code_traits<T> {
		static constexpr bool IS_ERROR_CODE {true};

		static constexpr auto isSuccess(T code) noexcept -> bool {
			return code == static_cast<T> (0);
		}

		static constexpr auto isFailure(T code) noexcept -> bool {
			return !isSuccess(code);
		}
	};


	template <typename T>
	constexpr auto is_error_code_v = error_code_traits<T>::IS_ERROR_CODE;

	template <typename T>
	concept error_code = is_error_code_v<T>;

} // namespace flex
