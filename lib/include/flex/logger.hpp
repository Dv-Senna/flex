#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <format>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>

#include "flex/reflection/reflection.hpp"


namespace flex {
	enum class LogLevel {
		eVerbose,
		eInfo,
		eWarning,
		eError,
		eFatal
	};

	class Logger {
		using FormatStringType = std::format_string<const std::string&, std::string&, std::string&, const std::string&>;

		public:
			Logger() noexcept = delete;
			Logger(const Logger&) noexcept = delete;
			auto operator=(const Logger&) noexcept -> Logger& = delete;
			Logger(Logger&&) noexcept = delete;
			auto operator=(Logger&&) noexcept -> Logger& = delete;


			template <typename ...Args>
			static inline auto verbose(const std::format_string<Args...> &format, Args &&...args) noexcept -> void {
				log(LogLevel::eVerbose, format, std::forward<Args> (args)...);
			}

			template <typename ...Args>
			static inline auto info(const std::format_string<Args...> &format, Args &&...args) noexcept -> void {
				log(LogLevel::eInfo, format, std::forward<Args> (args)...);
			}

			template <typename ...Args>
			static inline auto warning(const std::format_string<Args...> &format, Args &&...args) noexcept -> void {
				log(LogLevel::eWarning, format, std::forward<Args> (args)...);
			}

			template <typename ...Args>
			static inline auto error(const std::format_string<Args...> &format, Args &&...args) noexcept -> void {
				log(LogLevel::eError, format, std::forward<Args> (args)...);
			}

			template <typename ...Args>
			static inline auto fatal(const std::format_string<Args...> &format, Args &&...args) noexcept -> void {
				log(LogLevel::eFatal, format, std::forward<Args> (args)...);
			}


			template <typename ...Args>
			static auto log(LogLevel level, const std::format_string<Args...> &format, Args &&...args) noexcept -> void {
				if (level < s_minLevel)
					return;

				const std::string content {std::format(format, std::forward<Args> (args)...)};
				std::string levelString {};
				switch (level) {
					using enum LogLevel;
					case eVerbose: levelString = "verbose"; break;
					case eInfo: levelString = "info"; break;
					case eWarning: levelString = "warning"; break;
					case eError: levelString = "error"; break;
					case eFatal: levelString = "fatal"; break;
				}

				std::string colorStart {};
				if (s_colorEnabled) {
					switch (level) {
						using enum LogLevel;
						case eVerbose: colorStart = "\033[90m"; break;
						case eInfo: colorStart = "\033[34m"; break;
						case eWarning: colorStart = "\033[33m"; break;
						case eError: colorStart = "\033[91m"; break;
						case eFatal: colorStart = "\033[31m"; break;
					}
				}
				const std::string colorEnd {s_colorEnabled ? "\033[m" : ""};

				const std::string result {std::format(s_formatString,
					content,
					levelString,
					colorStart,
					colorEnd
				)};

				*s_outputStream << result << std::endl;
			}

			/*
			 * {0} : the content of the log
			 * {1} : the level of the log
			 * {2} : the function origin of the log
			 * {3} : the color opener
			 * {4} : the color resetter
			 * */
			static inline auto setFormatString(const FormatStringType &string) noexcept -> void {s_formatString = string;}

			static inline auto setOutputStream(std::ostream &stream, bool colorEnabled = false) noexcept -> void {
				s_outputStream = &stream;
				s_colorEnabled = colorEnabled;
			}

			static inline auto setMinLevel(LogLevel minLevel) noexcept -> void {s_minLevel = minLevel;}

		private:
			static std::ostream *s_outputStream;
			static FormatStringType s_formatString;
			static LogLevel s_minLevel;
			static bool s_colorEnabled;
	};



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
