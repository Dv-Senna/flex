#pragma once

#include "flex/reflection/enums.hpp"

#include <source_location>


namespace flex {
	namespace __internals {
		template <scoped_enumeration T, T VALUE>
		consteval auto getEnumName() noexcept -> std::optional<std::string_view> {
			using namespace std::string_view_literals;
			std::string_view name {std::source_location::current().function_name()};

		#if defined(__clang__)
			name = {name.begin() + name.find("VALUE = ") + "VALUE = "sv.size(), name.end()};
			if (*name.begin() == '(')
				return std::nullopt;
			name = {name.begin(), name.begin() + name.find("]")};
			name = {name.begin() + name.find("::") + "::"sv.size(), name.end()};
		#elif defined(__GNUC__)
			name = {name.begin() + name.find("VALUE = ") + "VALUE = "sv.size(), name.end()};
			if (*name.begin() == '(')
				return std::nullopt;
			name = {name.begin(), name.begin() + name.find("]")};
			name = {name.begin() + name.rfind("::") + "::"sv.size(), name.end()};
		#elif defined(_MSC_VER)
			name = {name.begin() + name.rfind(",") + ","sv.size(), name.end()};
			if (*name.begin() == '(')
				return std::nullopt;
			name = {name.begin() + name.rfind("::") + "::"sv.size(), name.end()};
			name = {name.begin(), name.begin() + name.find(">")};
		#endif

			return name;
		}


		template <unscoped_enumeration T, T VALUE>
		consteval auto getEnumName() noexcept -> std::optional<std::string_view> {
			using namespace std::string_view_literals;
			std::string_view name {std::source_location::current().function_name()};

		#if defined(__clang__)
			name = {name.begin() + name.find("VALUE = ") + "VALUE = "sv.size(), name.end()};
			if (*name.begin() == '(')
				return std::nullopt;
			name = {name.begin(), name.begin() + name.find("]")};
		#elif defined(__GNUC__)
			name = {name.begin() + name.find("VALUE = ") + "VALUE = "sv.size(), name.end()};
			if (*name.begin() == '(')
				return std::nullopt;
			name = {name.begin(), name.begin() + name.find("]")};
		#elif defined(_MSC_VER)
			name = {name.begin() + name.rfind(",") + ","sv.size(), name.end()};
			if (*name.begin() == '(')
				return std::nullopt;
			name = {name.begin(), name.begin() + name.find(">")};
		#endif

			return name;
		}
	}


	template <enumeration T>
	constexpr auto toString(T value) noexcept -> std::optional<std::string_view> {
		for (const auto &member : enum_members_v<T>) {
			if (member.value == value)
				return member.name;
		}
		return std::nullopt;
	}

} // namespace flex
