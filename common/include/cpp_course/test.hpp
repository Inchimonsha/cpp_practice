#pragma once

// Минимальный хедер-онли фреймворк для unit-тестов курса.
//
// Почему свой, а не gtest/doctest/Catch2:
//   * ноль внешних зависимостей — модуль common собирается «голым»;
//   * требует понимания языка: регистрация через статические объекты и
//     макросы — хорошая пища для разбора (см. th01, th11);
//   * достаточно для учебного курса.
//
// Зрелые фреймворки (doctest, Catch2, GoogleTest) рассматриваются в th49
// как рекомендуемая замена для коммерческой разработки.
//
// Использование:
//
//     #include <cpp_course/test.hpp>
//     CPP_COURSE_TEST("имя теста") {
//         CHECK(1 + 1 == 2);
//         CHECK_EQ(2, 1 + 1);
//         CHECK_NEAR(0.1f + 0.2f, 0.3f, 1e-5f);
//         CHECK_THROWS_AS(do_something(), std::runtime_error);
//     }
//
//     // в main:
//     return cpp_course::run_all_tests();

#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace cpp_course {

// Ошибка, сообщённая макросами CHECK*.
class test_failure : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Описание одного теста.
struct test_case {
    std::string name;
    std::function<void()> fn;
};

// Реестр тестов (глобальное состояние ограничено рамками фреймворка тестов).
inline std::vector<test_case>& test_registry() {
    static std::vector<test_case> registry;
    return registry;
}

// Регистратор теста: объект, создающийся при инициализации статической
// переменной (см. макрос CPP_COURSE_TEST).
struct test_registrar {
    test_registrar(std::string name, std::function<void()> fn) {
        test_registry().push_back({std::move(name), std::move(fn)});
    }
};

// Запускает все зарегистрированные тесты. Возвращает 0 при успехе, 1 иначе.
inline int run_all_tests() {
    int failed = 0;
    for (const auto& tc : test_registry()) {
        try {
            tc.fn();
        } catch (const std::exception& e) {
            ++failed;
            std::cerr << "[FAIL] " << tc.name << " : " << e.what() << '\n';
            continue;
        } catch (...) {
            ++failed;
            std::cerr << "[FAIL] " << tc.name << " : неизвестное исключение\n";
            continue;
        }
        std::cout << "[ OK ] " << tc.name << '\n';
    }
    const auto total = static_cast<int>(test_registry().size());
    std::cout << "----\n" << (total - failed) << '/' << total << " тестов пройдено\n";
    return failed == 0 ? 0 : 1;
}

// Разбивает условие-выражение в текст (для сообщений об ошибках).
inline std::string check_message(const char* expression, const char* file, int line) {
    std::ostringstream os;
    os << file << ':' << line << ": не выполнено: " << expression;
    return os.str();
}

}  // namespace cpp_course

// Двухуровневая склейка токенов: позволяет использовать __LINE__ в имени
// (одиночное ## не раскрывает __LINE__ как макрос).
#define CPP_COURSE_TEST_CAT_IMPL(a, b) a ## b
#define CPP_COURSE_TEST_CAT(a, b) CPP_COURSE_TEST_CAT_IMPL(a, b)

#define CPP_COURSE_TEST(name)                                                  \
    static void CPP_COURSE_TEST_CAT(cpp_course_test_fn_, __LINE__)();          \
    static ::cpp_course::test_registrar CPP_COURSE_TEST_CAT(                   \
        cpp_course_test_reg_, __LINE__)(                                       \
        name, &CPP_COURSE_TEST_CAT(cpp_course_test_fn_, __LINE__));            \
    static void CPP_COURSE_TEST_CAT(cpp_course_test_fn_, __LINE__)()

// CHECK принимает произвольное число аргументов: список аргументов макроса
// разбивается запятыми, вставленными внутри скобок и даже фигурных скобок
// (например, Vec2{3.0f, 4.0f}!). __VA_ARGS__ восстанавливает исходное
// выражение: запятые возвращаются на свои места.
#define CHECK(...)                                                             \
    do {                                                                       \
        if (!(__VA_ARGS__)) {                                                  \
            throw ::cpp_course::test_failure(                                  \
                ::cpp_course::check_message(#__VA_ARGS__, __FILE__, __LINE__)); \
        }                                                                      \
    } while (false)

#define CHECK_EQ(a, b)                                                         \
    do {                                                                       \
        const auto& _a = (a);                                                  \
        const auto& _b = (b);                                                  \
        if (!(_a == _b)) {                                                     \
            std::ostringstream _os;                                            \
            _os << __FILE__ << ':' << __LINE__ << ": " << #a << " == " << #b   \
                << " не выполнено";                                            \
            throw ::cpp_course::test_failure(_os.str());                       \
        }                                                                      \
    } while (false)

#define CHECK_NEAR(a, b, eps)                                                  \
    do {                                                                       \
        const auto _a = (a);                                                   \
        const auto _b = (b);                                                   \
        if (!(std::abs(_a - _b) <= (eps))) {                                   \
            std::ostringstream _os;                                            \
            _os << __FILE__ << ':' << __LINE__ << ": |" << #a << " - " << #b   \
                << "| <= " << #eps << " не выполнено (" << (_a - _b) << ')';   \
            throw ::cpp_course::test_failure(_os.str());                       \
        }                                                                      \
    } while (false)

#define CHECK_THROWS_AS(expr, expected_exception)                              \
    do {                                                                       \
        bool _caught = false;                                                  \
        try {                                                                  \
            (void)(expr);                                                      \
        } catch (const expected_exception&) {                                  \
            _caught = true;                                                    \
        } catch (...) {                                                        \
        }                                                                      \
        if (!_caught) {                                                        \
            throw ::cpp_course::test_failure(                                  \
                ::cpp_course::check_message(                                   \
                    #expr " должен бросить " #expected_exception,              \
                    __FILE__, __LINE__));                                      \
        }                                                                      \
    } while (false)
