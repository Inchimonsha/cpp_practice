#include <cpp_course/log.hpp>

#include <iostream>
#include <string>

namespace cpp_course {

const char* log_level_name(LogLevel level) {
    switch (level) {
        case LogLevel::trace: return "TRACE";
        case LogLevel::debug: return "DEBUG";
        case LogLevel::info:  return "INFO ";
        case LogLevel::warn:  return "WARN ";
        case LogLevel::error: return "ERROR";
    }
    return "?????";
}

Logger& Logger::instance() {
    static Logger global_logger;
    return global_logger;
}

Logger::Logger() : level_(LogLevel::info) {}

void Logger::set_prefix(std::string_view prefix) { prefix_.assign(prefix); }

void Logger::log(LogLevel level, std::string_view message) {
    if (static_cast<int>(level) < static_cast<int>(level_)) {
        return;
    }
    if (prefix_.empty()) {
        std::cerr << '[' << log_level_name(level) << "] " << message << '\n';
    } else {
        std::cerr << '[' << log_level_name(level) << "][" << prefix_ << "] "
                  << message << '\n';
    }
}

}  // namespace cpp_course