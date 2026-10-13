# m14_quality/ex04_static_analysis — статический анализ компилятора

> Модуль m14_quality · Сложность ★★ · C++23 · Запуск: `./run.ps1 m14_quality/ex04_static_analysis`
>
> Дополнительно проверен анализатор GCC: `g++ -fanalyzer` (GCC 16.2).

## Теория

**Статический анализ** ищет дефекты, не запуская код. Уровни:

1. **Компилятор**: предупреждения (`-Wall -Wextra -Wpedantic -Wshadow`),
   `-Werror` (предупреждение → ошибка), **`-fanalyzer`** (символическое
   исполнение: двойное освобождение, утечки, null-deref).
2. **Внешние линтеры**: `clang-tidy`, `cppcheck`, `include-what-you-use`.
3. **Языковые средства**: `static_assert`, `consteval`, `[[nodiscard]]`,
   `[[maybe_unused]]`, `[[deprecated]]`, `if consteval` — переносят часть
   проверок в компиляцию.

Чем раньше найден дефект, тем дешевле исправление: компиляция < тесты <
продакшн.

## Синтаксис

```cpp
[[nodiscard]] constexpr int validated_multiply(int a, int b) { return a * b; }

consteval std::array<int, 5> first_squares() { /* ... */ return result; }

constexpr bool table_valid() { /* ... */ }
static_assert(table_valid(), "tablica kvadratov neverna");

template <class T> [[nodiscard]] constexpr std::size_t type_size() { return sizeof(T); }

int evaluation_phase() { if consteval { return 1; } else { return 0; } }
```

Проверка анализатором (проверено на этом GCC):

```bash
g++ -std=c++23 -fanalyzer -c file.cpp
# warning: double-'free' of 'p' [CWE-415] [-Wanalyzer-double-free]
```

## Подводные камни

- **`[[nodiscard]]` не спасает**, если результат всё равно используется
  формально; следите, чтобы возврат не игнорировали (`static_cast<void>` —
  осознанное подавление).
- **`consteval` форсирует compile-time**: если вычисление невозможно на этапе
  компиляции, это ошибка компиляции — не всякая функция подойдёт.
- **`if consteval` (C++23)** требует именно runtime-контекст для ветки `else`;
  в не-`constexpr` функции `if consteval` = false.
- **`-fanalyzer` медленный и неполный**: может давать ложные срабатывания и
  пропуски; дополняйте санитайзерами (ASan/UBSan) и тестами.
- **`static_assert` не видит runtime-данные** — только compile-time константы.
- Внешние линтеры надо **настраивать** (`clang-tidy -checks=`), иначе шум
  перекроет полезные сигналы.

## Задача

Продемонстрировать compile-time проверки: `[[nodiscard]]`-умножение,
`consteval`-таблицу квадратов с `static_assert`, `[[maybe_unused]]`,
`if consteval`. Вывести результаты. Дополнительно — `-fanalyzer` (см. выше).

## Где применяется

- Обязательный этап CI для C/C++/Rust, безопасный код (MISRA/AUTOSAR),
  рефакторинг больших кодовых баз, поиск UB до релиза.

## Проверка

Вывод: `compile-time validation passed`, `validated_product = 42`,
`squares[4] = 16`, `int size = 4`, `evaluation phase = 0 (runtime)`,
`static analysis done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex04_static_analysis.cpp`. Проверка анализатора:
`g++ -fanalyzer` на коде с двойным `free` выдаёт
`warning: double-'free' ... [-Wanalyzer-double-free]` (проверено на GCC 16.2).
