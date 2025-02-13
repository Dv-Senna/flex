#pragma once

#include <compare>
#include <cstdint>


namespace flex {
	struct Version {
		std::uint8_t major;
		std::uint8_t minor;
		std::uint8_t patch;

		constexpr auto operator<=> (const Version &version) const noexcept -> std::strong_ordering {
			const std::uint32_t lhs {(static_cast<std::uint32_t> (major) << 16)
				| (static_cast<std::uint32_t> (minor) << 8)
				| static_cast<std::uint32_t> (patch)
			};
			const std::uint32_t rhs {(static_cast<std::uint32_t> (version.major) << 16)
				| (static_cast<std::uint32_t> (version.minor) << 8)
				| static_cast<std::uint32_t> (version.patch)
			};
			return lhs <=> rhs;
		}
	};


	enum class OS {
		eLinux,
		eWindows,
		eMacos
	};

	enum class Compiler {
		eGCC,
		eClang,
		eMSVC
	};

	enum class BuildType {
		eDebug,
		eRelease
	};

	enum class CppStandard {
		e17,
		e20,
		e23
	};

	static_assert(flex::CppStandard::e23 > flex::CppStandard::e20);
	static_assert(flex::CppStandard::e20 > flex::CppStandard::e17);

} // namespace flex
