// Главная точка входа для тестов общего модуля.

#include <cpp_course/log.hpp>
#include <cpp_course/test.hpp>

int main() {
    cpp_course::log().set_prefix("common_tests");
    return cpp_course::run_all_tests();
}