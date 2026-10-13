# ex05_optimization_patterns — паттерны оптимизации

## Теория

Оптимизации, которые стоит применять осознанно (после измерений):

- **Копийная элизия / RVO**: возврат по значению не создаёт копию. Для prvalue
  (`return T{};`) элизия **гарантирована** с C++17; для именованного NRVO —
  обычно применяется на `-O2`.
- **`inline`/`constexpr`**: позволяют встраивать код; не гарантируют инлайн, но
  убирают барьеры. `constexpr`-функции считаемы на этапе компиляции.
- **Подсказки о ветвлениях**: `[[likely]]`/`[[unlikely]]` (C++20) и
  `__builtin_expect` (GNU) сообщают вероятное направление ветки. Помогают
  укладывать горячий путь без переходов.
- **Малые функции**: короткие тела легче инлайнятся и оптимизируются.
- **Правило**: сначала профилируйте (ex04), потом оптимизируйте. Преждевременная
  оптимизация вредит читаемости.

## Синтаксис

```cpp
if (cond) [[likely]]   { hot_path(); }
else        [[unlikely]] { cold_path(); }

if (__builtin_expect(cond, 0)) { /* redkij sluchaj */ }

T make() { return T{}; }        // garantirovannaya kopijnaya eliziya
```

## Подводные камни

- `[[likely]]`/`__builtin_expect` — лишь **подсказки`; при плохом предсказании
  профиля могут навредить.
- `inline` не гарантирует встраивание; и наоборот, не помеченные функции могут
  инлайниться.
- RVO нельзя «увидеть» в замерах без счётчиков копий — используйте инструментный
  счётчик, как здесь.
- Преждевременные микрооптимизации усложняют код и часто не дают выигрыша.
- `__builtin_expect` — расширение GCC/Clang, не портативно.

## Задача

1. Показать гарантированную копийную элизию: фабрика `make_tracker()` возвращает
   prvalue; счётчик копий обязан остаться `0`.
2. Сумма абсолютных значений вектора `{1,-2,3,-4,5}` двумя способами: с
   `[[likely]]` и с `__builtin_expect`; обе дают `15`.

## Где применяется

- Горячие циклы, парсеры, интерпретаторы, обработка потоков данных; предсказуемые
  ветки в коде проверки/ошибок.

## Проверка

Вывод: `copies = 0`, обе суммы `15`; код возврата `0` при выполнении обоих
условий, иначе `1`.

## Решение

```cpp
#include <iostream>
#include <print>
#include <vector>

namespace {
struct Tracker {
    static inline int copies = 0;
    int id;
    explicit Tracker(int value) : id(value) {}
    Tracker(const Tracker& other) : id(other.id) { ++copies; }
    Tracker(Tracker&& other) noexcept : id(other.id) {}
};

Tracker make_tracker(int id) {
    return Tracker{id};   // garantirovannaya eliziya (C++17)
}

int sum_likely(const std::vector<int>& values) {
    int sum = 0;
    for (int x : values) {
        if (x >= 0) [[likely]] {
            sum += x;
        } else {
            sum -= x;
        }
    }
    return sum;
}

int sum_expect(const std::vector<int>& values) {
    int sum = 0;
    for (int x : values) {
        if (__builtin_expect(x < 0, 0)) {
            sum -= x;
        } else {
            sum += x;
        }
    }
    return sum;
}
}  // namespace

int main() {
    Tracker tracker = make_tracker(7);
    std::println("RVO copies = {} (expected 0), id = {}",
                 Tracker::copies, tracker.id);

    const std::vector<int> values{1, -2, 3, -4, 5};
    const int a = sum_likely(values);
    const int b = sum_expect(values);
    std::println("sum_likely = {} sum_expect = {}", a, b);

    std::cout << "optimization patterns done\n";
    return (Tracker::copies == 0 && a == 15 && b == 15) ? 0 : 1;
}
```
