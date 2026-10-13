# ex04_unit_tests_fuzz — тесты, свойства и фаззинг

## Теория

Качество обеспечивают три дополняющих подхода:

- **Модульные тесты**: фиксированные примеры на конкретное поведение
  (вход → ожидаемый выход). Быстрые, точные, но покрывают только придуманные
  случаи.
- **Свойства (property-based)**: вместо примеров проверяют инвариант на множестве
  входов (в т.ч. случайных с фиксированным seed ради воспроизводимости).
- **Фаззинг**: подача большого числа случайных/полу-структурированных входов для
  поиска падений и UB. Инструмент — libFuzzer (`-fsanitize=fuzzer`) или AFL.

Хорошая стратегия — все три вместе, плюс санитайзеры (ASan/UBSan) в CI.

## Синтаксис

```cpp
int checks = 0, failures = 0;
#define CHECK(cond) do { ++checks; if (!(cond)) { ++failures; report(#cond); } } while (0)

// property s fiksirovannym seed
std::mt19937 rng(12345);
CHECK(clamp(v, 0, 10) >= 0);

// fuzzing (clang/GCC na Linux):
// g++ -fsanitize=fuzzer,address fuzz.cpp -o fuzz && ./fuzz
```

## Подводные камни

- Случайные тесты должны иметь **фиксированный seed**, иначе воспроизводимость
  теряется.
- Тест, который всегда истинен (тавтология), бесполезен — проверяйте настоящие
  инварианты.
- `-fsanitize=fuzzer` и `libFuzzer` недоступны в MinGW-сборке GCC; здесь
  используется собственный цикл фаззинга, а инструментальный фаззинг
  описывается для Linux/clang.
- Падение внутри фаззера надо минимизировать до короткого входа и превращать в
  регрессионный модульный тест.
- Тесты требуют изоляции: глобальное состояние между тестами ломает результаты.

## Задача

Мини-набор тестов для функций `clamp(v, lo, hi)` и `parse_uint(string_view)`
(возвращает `-1` при пустой строке, нецифре или значении `> 1'000'000`):

1. примеры для `clamp` и `parse_uint`;
2. свойство `clamp` на 1000 случайных значениях (seed `12345`);
3. фаззинг `parse_uint` на 2000 случайных строк (seed `777`): нет падений,
   успешный разбор возможен только из цифр.

## Где применяется

- Регрессионная защита библиотек и игровой логики, проверка парсеров/форматов,
  безопасность (фаззинг сериализации/сетевых протоколов).

## Проверка

Вывод: количество проверок и `failures = 0`; код возврата `0`, если провалов нет.

## Решение

```cpp
#include <iostream>
#include <print>
#include <random>
#include <string>
#include <string_view>

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool cond, const char* expr, int line) {
    ++g_checks;
    if (!cond) {
        ++g_failures;
        std::println("  FAIL line {}: {}", line, expr);
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

int clamp(int value, int lo, int hi) {
    return value < lo ? lo : (value > hi ? hi : value);
}

int parse_uint(std::string_view text) {
    if (text.empty()) { return -1; }
    long long value = 0;
    for (char ch : text) {
        if (ch < '0' || ch > '9') { return -1; }
        value = value * 10 + (ch - '0');
        if (value > 1000000) { return -1; }
    }
    return static_cast<int>(value);
}

void test_clamp_examples() {
    CHECK(clamp(5, 0, 10) == 5);
    CHECK(clamp(-3, 0, 10) == 0);
    CHECK(clamp(42, 0, 10) == 10);
}

void test_clamp_property() {
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(-100, 100);
    for (int i = 0; i < 1000; ++i) {
        const int v = dist(rng);
        const int c = clamp(v, 0, 10);
        CHECK(c >= 0 && c <= 10);
        CHECK(c == v || c == 0 || c == 10);
    }
}

void test_parse_examples() {
    CHECK(parse_uint("0") == 0);
    CHECK(parse_uint("123") == 123);
    CHECK(parse_uint("") == -1);
    CHECK(parse_uint("12a") == -1);
    CHECK(parse_uint("9999999") == -1);
}

void fuzz_parse() {
    std::mt19937 rng(777);
    std::uniform_int_distribution<int> len(0, 8);
    std::uniform_int_distribution<int> chr(32, 126);
    for (int i = 0; i < 2000; ++i) {
        std::string text;
        const int n = len(rng);
        for (int j = 0; j < n; ++j) {
            text.push_back(static_cast<char>(chr(rng)));
        }
        const int result = parse_uint(text);
        if (result != -1) {
            bool all_digits = true;
            for (char ch : text) {
                if (ch < '0' || ch > '9') { all_digits = false; }
            }
            CHECK(all_digits);
            CHECK(result >= 0 && result <= 1000000);
        }
    }
}

}  // namespace

int main() {
    std::println("running tests:");
    test_clamp_examples();
    test_clamp_property();
    test_parse_examples();
    fuzz_parse();

    std::println("checks = {} failures = {}", g_checks, g_failures);
    std::cout << "tests/fuzz done\n";
    return g_failures == 0 ? 0 : 1;
}
```
