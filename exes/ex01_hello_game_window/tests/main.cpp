// Точка входа тестов: запускает все зарегистрированные CPP_COURSE_TEST.
// Возвращает 0 при успехе, 1 — если хоть один тест упал.

#include <cpp_course/test.hpp>

int main() {
    return cpp_course::run_all_tests();
}