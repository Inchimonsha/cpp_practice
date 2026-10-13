# m10_language_future/ex02_reflection — рефлексия (C++26 и её эмуляция)

## Теория

Рефлексия даёт программе доступ к **собственной структуре** на этапе компиляции.
C++26 вводит оператор `^^` (reflection operator) и библиотеку `std::meta`:

```cpp
constexpr std::meta::info type = ^^Player;      // «отражение» типа
constexpr auto fields = std::meta::nonstatic_data_members_of(type);
// имена, типы, обход полей — во время компиляции
```

Это позволяет автоматически генерировать сериализацию, сравнение, ORM-маппинги и
биндинги **без макросов и ручных специализаций**.

В GCC 16.2 ни `^^`, ни `std::meta` ещё нет. Но идею структурной рефлексии можно
воспроизвести на C++20/23 через **structured bindings**: агрегатные типы
«разбираются» на поля, а число полей определяется через `requires` (техника
Boost.PFR). Имена полей пока приходится задавать явно — это и есть та ручная
работа, которую уберёт `^^`.

## Синтаксис

```cpp
struct any { template <class T> constexpr operator T() const; };

template <class T, class... Args>
consteval std::size_t fields_count_impl() {
    if constexpr (requires { T{ Args{}..., any{} }; })      // "eshchyo odno pole?"
        return fields_count_impl<T, Args..., any>();
    else
        return sizeof...(Args);                              // spustilis' do dna
}

template <class T>
constexpr auto to_tuple(T&& object) {
    if constexpr (fields_count<T>() == 2) {
        auto&& [a, b] = object;                              // structured binding
        return std::forward_as_tuple(a, b);
    }
}
```

## Подводные камни

- Техника работает только для **агрегатов** без базовых классов, приватных
  полей и пользовательских конструкторов.
- `struct any` с шаблонным `operator T()` объявляется **без тела** — важен лишь в
  невычисленном контексте `requires`.
- `requires { T{ Args{}..., any{} }; }` для агрегата требует
  default-constructible полей.
- `fields_count` и `to_tuple` должны покрывать нужное число полей вручную
  (здесь — до 4); полноценный PFR разворачивает это на шире.
- Порядок полей в structured bindings = порядок объявления; `^^` даёт то же, но
  с именами и типами, без ручных `if constexpr`.
- Имена полей сопоставляются по индексу — рассинхрон специализации и struct ловит
  только тест, а не компилятор.

## Задача

Реализовать `fields_count<T>()` (автоопределение числа полей агрегата) и
`to_tuple(obj)` (разбор агрегата на `std::tuple` ссылок через structured
bindings), плюс `dump(obj)` — сериализацию в «JSON-подобную» строку с именами
полей из специализации `field_names<T>`. Продемонстрировать на `Point{x,y}` и
`Player{id,name,hp}`.

## Где применяется

- Сериализация/десериализация без ручного кода; ORM; автоматические
  `operator==`, `hash`, `to_string`; CLI/UI-биндинги; тестовые фабрики.
  Основа: Boost.PFR, Boost.Hana, magic_enum; в будущем — `std::meta` (C++26).

## Проверка

Вывод: `fields_count` (`2` и `3`), строки `{"x":1, "y":2}` и
`{"id":7, "name":"neo", "hp":100}`, тег режима (reflection/fallback). Код
возврата `0` при совпадении, иначе `1`.

## Решение

```cpp
#include <array>
#include <cstddef>
#include <format>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace reflect {

struct any {
    template <class T>
    constexpr operator T() const;
};

template <class T, class... Args>
consteval std::size_t fields_count_impl() {
    if constexpr (requires { T{ Args{}..., any{} }; }) {
        return fields_count_impl<T, Args..., any>();
    } else {
        return sizeof...(Args);
    }
}

template <class T>
consteval std::size_t fields_count() {
    return fields_count_impl<std::remove_cvref_t<T>>();
}

template <class T>
constexpr auto to_tuple(T&& object) {
    using U = std::remove_cvref_t<T>;
    if constexpr (fields_count<U>() == 0) {
        return std::tuple<>{};
    } else if constexpr (fields_count<U>() == 1) {
        auto&& [a] = object;
        return std::forward_as_tuple(a);
    } else if constexpr (fields_count<U>() == 2) {
        auto&& [a, b] = object;
        return std::forward_as_tuple(a, b);
    } else if constexpr (fields_count<U>() == 3) {
        auto&& [a, b, c] = object;
        return std::forward_as_tuple(a, b, c);
    } else {
        auto&& [a, b, c, d] = object;
        return std::forward_as_tuple(a, b, c, d);
    }
}

}  // namespace reflect

struct Point {
    double x;
    double y;
};

struct Player {
    int id;
    std::string name;
    double hp;
};

template <class T>
struct field_names;

template <>
struct field_names<Point> {
    static constexpr std::array<std::string_view, 2> names{"x", "y"};
};

template <>
struct field_names<Player> {
    static constexpr std::array<std::string_view, 3> names{"id", "name", "hp"};
};

template <class V>
std::string format_value(const V& value) {
    if constexpr (std::is_convertible_v<V, std::string_view>) {
        return std::format("\"{}\"", std::string_view(value));
    } else {
        return std::format("{}", value);
    }
}

template <class T, class Tuple, std::size_t... I>
std::string dump_impl(const Tuple& values, std::index_sequence<I...>) {
    std::string out = "{";
    std::size_t written = 0;
    ((out += std::string(written++ ? ", " : "") +
             std::format("\"{}\":{}", field_names<T>::names[I],
                         format_value(std::get<I>(values)))),
     ...);
    return out + "}";
}

template <class T>
std::string dump(const T& object) {
    return dump_impl<T>(reflect::to_tuple(object),
                        std::make_index_sequence<reflect::fields_count<T>()>{});
}

int main() {
#if defined(__cpp_lib_reflection) || defined(__cpp_reflection)
    std::println("mode = std::meta (C++26)");
#else
    std::println("mode = fallback (structured bindings; GCC 16.2 bez ^^/std::meta)");
#endif

    std::println("fields(Point) = {}", reflect::fields_count<Point>());
    std::println("fields(Player) = {}", reflect::fields_count<Player>());

    const Point point{1.0, 2.0};
    const Player player{7, "neo", 100.0};

    const std::string point_text = dump(point);
    const std::string player_text = dump(player);
    std::println("point = {}", point_text);
    std::println("player = {}", player_text);

    const bool ok = reflect::fields_count<Point>() == 2 &&
                    reflect::fields_count<Player>() == 3 &&
                    point_text == R"({"x":1, "y":2})" &&
                    player_text == R"({"id":7, "name":"neo", "hp":100})";
    std::cout << "reflection done\n";
    return ok ? 0 : 1;
}
```
