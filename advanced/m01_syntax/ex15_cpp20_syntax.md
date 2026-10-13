# m01_syntax/ex15_cpp20_syntax — Синтаксис C++20

> **Модуль:** `m01_syntax` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m01_syntax/ex15_cpp20_syntax`

## Теория (кратко)

C++20 — крупнейший релиз после C++11: concepts, ranges, `std::format`,
`std::span`, `<=>`, designated initializers, `constinit`, `using enum`. Многое из
этого вы уже видели по курсу; здесь соберём вместе и осознанно.

| Возможность | Выигрыш |
|---|---|
| concepts/`requires` | читаемые ограничения шаблонов, понятные ошибки |
| ranges/views | композиция алгоритмов без временных контейнеров |
| `std::format` | типобезопасный формат вместо `printf`/`<<` |
| `std::span` | единый параметр для любого непрерывного буфера |
| `<=>` | автоматические сравнения |
| designated init | именованные поля при инициализации |
| `using enum` | импорт значений `enum class` в область видимости |
| `constinit` | гарантия статической инициализации |

## Синтаксис

```cpp
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
T sum(std::span<const T> values);

const std::string s = std::format("{} {}", 42, "items");
auto evens = data | std::views::filter([](int v){ return v % 2 == 0; });

enum class Color { red, green };
using enum Color;        // teper' "red" dostupen bez Color::
```

## Подводные камни

- `<format>`/`format` на старых компиляторах требует отдельной библиотеки
  `{fmt}`; на GCC 13+ уже есть.
- Range-views ленивы: цепочка ничего не делает, пока её не «прожуёт» цикл или
  `ranges::to`.
- `std::span` не владеет данными.
- `using enum` может привести к конфликтам имён — применяйте локально.

## Задача

В `ex15_cpp20_syntax.cpp`:

1. определите `concept Numeric` и функцию `sum(std::span<const T>)`,
   ограниченную им;
2. отфильтруйте чётные через `std::views::filter` и выведите;
3. соберите строку через `std::format`;
4. просуммируйте `std::span` от `std::vector` и от C-массива;
5. используйте `using enum` и `constinit`.

## Где применяется

- Concepts — в API шаблонных библиотек для внятных сообщений об ошибках.
- Views — конвейеры обработки данных без промежуточных контейнеров.
- `std::format` — логи, отчёты, генерация текста; на MinGW требует `-lstdc++exp`.

## Проверка

Прогон печатает суммы и отфильтрованные значения; завершается `0`.

### Решение

См. `ex15_cpp20_syntax.cpp`.
