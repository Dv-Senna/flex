#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <format>
#include <iomanip>
#include <optional>
#include <ranges>
#include <sstream>

#include "flex/reflection/reflection.hpp"


namespace flex {
	template <std::integral T>
	constexpr auto stoi(std::string_view str) -> std::optional<T> {
		T result {};
		T position {1};
		for (char digit : str) {
			if (digit < '0' || digit > '9')
				return std::nullopt;
			result += static_cast<T> (digit - '0') * position;
			position *= static_cast<T> (10);
		}
		return result;
	}


	namespace __internals {
		template <flex::reflectable T, std::size_t INDEX = 0>
		requires (INDEX < flex::reflection_members_count_v<T>)
		auto logReflectableMember(std::ostringstream &stream, const T &value, std::size_t width) noexcept -> void {
			std::string spaces (width, ' ');
			stream << spaces << std::get<INDEX> (flex::reflection_members_names_v<T>) << ": ";

		#define __FLEX_CURRENT_VALUE static_cast<const std::tuple_element_t<INDEX, flex::reflection_members_t<T>>&> (flex::reflection_traits<T>::template getMember<INDEX> (value))
			if constexpr (!flex::is_reflectable_v<std::tuple_element_t<INDEX, flex::reflection_members_t<T>>>) {
				if constexpr (flex::is_string_v<std::tuple_element_t<INDEX, flex::reflection_members_t<T>>>)
					stream << std::quoted(__FLEX_CURRENT_VALUE);
				else
					stream << __FLEX_CURRENT_VALUE;
			}
			else {
				if (width == 0)
					stream << std::format("{}", __FLEX_CURRENT_VALUE);
				else {
					std::string format {std::format("{{:{}}}", width)};
					format = std::vformat(format, std::make_format_args(__FLEX_CURRENT_VALUE));
					for (auto it {format.begin()}; it != format.end(); ++it) {
						if (*it != '\n')
							continue;
						it = format.insert(it+1, width, ' ') + width;
					}
					stream << format;
				}
			}

			if constexpr (INDEX + 1 < reflection_members_count_v<T>)
				stream << ", ";
			if (width != 0)
				stream << "\n";
			if constexpr (INDEX + 1 < reflection_members_count_v<T>)
				logReflectableMember<T, INDEX+1> (stream, value, width);
		#undef __FLEX_CURRENT_VALUE
		}

	} // namespace __internals

} // namespace flex


template <flex::reflectable T>
class std::formatter<T, char> {
	public:
		constexpr auto parse(std::format_parse_context &ctx) -> std::format_parse_context::iterator {
			m_width = 0;
			auto it {ctx.begin()};
			if (it == ctx.end())
				return it;

			std::string_view args {ctx.begin(), ctx.end() - 1};
			if (args.empty())
				return ctx.end() - 1;
			std::optional<std::size_t> widthWithError {flex::stoi<std::size_t> (args)};
			if (!widthWithError)
				throw std::format_error("The format part of reflectable must be a number");
			m_width = *widthWithError;
			return ctx.end() - 1;
		}


		auto format(const T &value, std::format_context &ctx) const noexcept -> std::format_context::iterator {
			std::ostringstream stream {};
			stream << "{";
			if (m_width != 0)
				stream << "\n";
			flex::__internals::logReflectableMember(stream, value, m_width);
			stream << "}";
			return std::ranges::copy(std::move(stream).str(), ctx.out()).out;
		};

	private:
		std::size_t m_width;
};
