#pragma once

// Минимальный логгер курса.
//
// Область применения: диагностика в примерах, упражнениях и проектах.
// Намеренно простой:
//   * уровни Trace..Error;
//   * вывод в stderr с общим префиксом и префиксом уровня;
//   * глобальный экземпляр (см. threading note ниже).
//
// Для коммерческой разработки рекомендуется зрелое решение (spdlog и т.п.) —
// см. th50_profiling_debugging.

#include <string>
#include <string_view>

namespace cpp_course {

enum class LogLevel {
    trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    error = 4,
};

// Имя уровня для печати.
const char* log_level_name(LogLevel level);

class Logger {
public:
    // Единственный экземпляр. Глобальное изменяемое состояние оправдано
    // здесь: логгер — инфраструктурная точка доступа, приборов много,
    // а инъекция логгера в каждую цель учебного курса усложнила бы код без
    // пользы для обучения. Для продакшен-кода смотрите service locator /
    // dependency injection (th32) и их ограничения.
    static Logger& instance();

    Logger();

    LogLevel level() const { return level_; }
    void set_level(LogLevel level) { level_ = level; }

    // Текстовый префикс, добавляемый перед сообщением (например, имя модуля).
    void set_prefix(std::string_view prefix);

    // Логирует сообщение, если оно не ниже текущего уровня.
    void log(LogLevel level, std::string_view message);

    void trace(std::string_view message) { log(LogLevel::trace, message); }
    void debug(std::string_view message) { log(LogLevel::debug, message); }
    void info(std::string_view message)  { log(LogLevel::info, message); }
    void warn(std::string_view message)  { log(LogLevel::warn, message); }
    void error(std::string_view message) { log(LogLevel::error, message); }

private:
    LogLevel level_;
    std::string prefix_;
};

// Удобный доступ:  cpp_course::log::info("...").
inline Logger& log() { return Logger::instance(); }

}  // namespace cpp_course