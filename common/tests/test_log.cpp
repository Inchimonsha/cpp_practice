// Тесты логгера: проверяем уровень и имена уровней.
// Вывод в stderr в unit-тестах не проверяется (это интеграционный сценарий).

#include <cpp_course/log.hpp>
#include <cpp_course/test.hpp>

#include <string>

CPP_COURSE_TEST("Logger: уровень по умолчанию info") {
    auto& logger = cpp_course::Logger::instance();
    CHECK(logger.level() == cpp_course::LogLevel::info);
    logger.set_level(cpp_course::LogLevel::debug);
    CHECK(logger.level() == cpp_course::LogLevel::debug);
    logger.set_level(cpp_course::LogLevel::info);  // возврат к умолчанию
}

CPP_COURSE_TEST("Logger: имена уровней") {
    CHECK_EQ(std::string(cpp_course::log_level_name(cpp_course::LogLevel::trace)), "TRACE");
    CHECK_EQ(std::string(cpp_course::log_level_name(cpp_course::LogLevel::debug)), "DEBUG");
    CHECK_EQ(std::string(cpp_course::log_level_name(cpp_course::LogLevel::info)), "INFO ");
    CHECK_EQ(std::string(cpp_course::log_level_name(cpp_course::LogLevel::warn)), "WARN ");
    CHECK_EQ(std::string(cpp_course::log_level_name(cpp_course::LogLevel::error)), "ERROR");
}