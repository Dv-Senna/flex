#include "flex/logger.hpp"


namespace flex {
	std::ostream *Logger::s_outputStream {&std::cout};
	Logger::FormatStringType Logger::s_formatString {"{2}[{1}] > {0}{3}"};
	LogLevel Logger::s_minLevel {LogLevel::eInfo};
	bool Logger::s_colorEnabled {true};

} // namespace flex
