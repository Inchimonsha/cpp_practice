# m10_language_future/ex03_pattern_matching — сопоставление с образцом

## Теория

Сопоставление с образцом (pattern matching) проверяет **форму** значения и
одновременно связывает его части. В C++26 для этого вводится `inspect`:

```cpp
double area(const Shape& shape) {
    return inspect (shape) {
        is Circle r  => std::numbers::pi * r * r,
        is Square s  => s * s,
        is Rect w, h => w * h,
    };
}
```

Компилятор проверяет **исчерпывающность** (exhaustiveness) и даёт доступ к
полям, а не только к типу.

В GCC 16.2 `inspect`/`is` нет. Стандартная замена на C++17/20 —
`std::visit` со «перегруженным» visitor'ом (overloaded visitor). Это даёт разбор
по типам варианта, но **без** проверки исчерпывающности и без «связывания с
именем» в синтаксисе образца.

## Синтаксис

```cpp
template <class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;       // deduction guide (C++17)

double area(const Shape& s) {
    return std::visit(overloaded{
        [](const Circle& c) { return pi * c.r * c.r; },
        [](const Square& q) { return q.side * q.side; },
        [](const Rect& r)   { return r.w * r.h; },
    }, s);
}
```

## Подводные камни

- `std::visit` **не** проверяет исчерпывающность: забытая альтернатива — ошибка
  компиляции только если visitor не вызывается ни для одной альтернативы; часто
  добавляют `[](const auto&) { ... }` как «default».
- Возвращаемые типы всех ветвей должны совпадать (иначе не выведется `auto`).
- Порядок и типы альтернатив `std::variant` задают приоритет; одинаковые типы
  допустимы, но различать их через `visit` нельзя.
- `std::get_if<T>(&v)` возвращает указатель, а не ссылку — удобно для «одной»
  ветви, но не заменяет полный разбор.
- В C++26 `inspect` даёт связывание и проверку полноты; ручной `visit` требует
  дисциплины.

## Задача

Написать `area` (площадь через `std::visit` + `overloaded`) и `describe`
(классификация с «охранным» условием: квадрат со стороной ≥ 2 — «large», иначе
«small») для `std::variant<Circle, Square, Rect>`. Дополнительно — `try_circle`,
возвращающий радиус через `std::get_if` (`std::optional`). Вывести результаты.

## Где применяется

- Разбор AST/токенов, обработка сообщений протоколов, редукция состояний,
  парсинг команд CLI, обработка `std::variant`/`std::expected`-результатов.

## Проверка

Вывод: режим (fallback/`inspect`), площади (`Circle r=2` → `12.5664`,
`Square 3` → `9`, `Rect 2x5` → `10`), классификации (`large`/`small`), `try_circle`
(`3`/`nullopt`). Код возврата `0` при совпадении, иначе `1`.

## Решение

```cpp
#include <cmath>
#include <format>
#include <iostream>
#include <numbers>
#include <optional>
#include <print>
#include <string>
#include <variant>

struct Circle { double r; };
struct Square { double side; };
struct Rect { double w; double h; };

using Shape = std::variant<Circle, Square, Rect>;

template <class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

double area(const Shape& shape) {
    return std::visit(overloaded{
        [](const Circle& c) { return std::numbers::pi * c.r * c.r; },
        [](const Square& q) { return q.side * q.side; },
        [](const Rect& r) { return r.w * r.h; },
    }, shape);
}

std::string describe(const Shape& shape) {
    return std::visit(overloaded{
        [](const Circle& c) { return "circle r=" + std::format("{}", c.r); },
        [](const Square& q) {
            return std::string(q.side >= 2.0 ? "large square" : "small square");
        },
        [](const Rect& r) { return "rect " + std::format("{}x{}", r.w, r.h); },
    }, shape);
}

std::optional<double> try_circle(const Shape& shape) {
    if (const auto* c = std::get_if<Circle>(&shape)) {
        return c->r;
    }
    return std::nullopt;
}

int main() {
#if defined(__cpp_pattern_matching)
    std::println("mode = inspect (C++26)");
#else
    std::println("mode = fallback (std::visit; GCC 16.2 bez inspect)");
#endif

    const Shape circle = Circle{2.0};
    const Shape square = Square{3.0};
    const Shape rect = Rect{2.0, 5.0};

    std::println("area(circle) = {:.4f}", area(circle));
    std::println("area(square) = {:.4f}", area(square));
    std::println("area(rect) = {:.4f}", area(rect));
    std::println("describe(circle) = {}", describe(circle));
    std::println("describe(square) = {}", describe(square));
    std::println("describe(rect) = {}", describe(rect));

    const auto radius = try_circle(circle);
    std::println("try_circle(circle) = {}", radius ? std::format("{}", *radius) : "nullopt");
    std::println("try_circle(rect) = {}",
                 try_circle(rect) ? std::format("{}", *try_circle(rect)) : "nullopt");

    const bool ok = std::abs(area(circle) - 12.566370614) < 1e-6 &&
                    area(square) == 9.0 && area(rect) == 10.0 &&
                    describe(square) == "large square" &&
                    radius.has_value() && *radius == 2.0 && !try_circle(rect).has_value();
    std::cout << "pattern matching done\n";
    return ok ? 0 : 1;
}
```

`<cmath>` нужен для `std::abs`; в полном файле он подключён.
