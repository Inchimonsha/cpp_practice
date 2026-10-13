# m01_syntax/ex16_cpp23_syntax — Синтаксис C++23

> **Модуль:** `m01_syntax` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m01_syntax/ex16_cpp23_syntax`

## Теория (кратко)

C++23 доводит до практичности то, что C++20 только наметил: `std::expected` для
ошибок без исключений, `std::flat_map`, `std::mdspan` для многомерных данных,
`std::print`/`println`, `views::enumerate`, `ranges::to`, `std::byteswap`,
`if consteval`, многомерный `operator[]`.

| Возможность | Зачем |
|---|---|
| `std::expected<T,E>` | значение или ошибка, без исключений |
| `views::enumerate` | индекс + элемент в цикле |
| `ranges::to<C>` | собрать диапазон в контейнер |
| `std::byteswap` | смена порядка байт (сеть/форматы) |
| `if consteval` | разное поведение в compile/runtime |
| `operator[]` с несколькими индексами | многомерный доступ |
| `std::mdspan` | невладеющее многомерное представление |

## Синтаксис

```cpp
std::expected<int, std::string> parse(std::string_view s);  // value ili oshibka

for (auto [i, v] : std::views::enumerate(container)) { use(i, v); }

auto out = data | std::views::filter(pred) | std::ranges::to<std::vector>();

std::uint32_t swapped = std::byteswap(value);
```

## Подводные камни

- `std::print`/`std::println` на MinGW-GCC требуют линковки `-lstdc++exp`
  (скрипт добавляет её). На Linux — тоже доступна.
- `std::expected::value()` бросает `bad_expected_access`, если ошибка; для
  «мягкого» доступа — `value_or` / `and_then` / `transform` (`m05`).
- `views::enumerate` даёт «индекс-элемент» как пару; тип индекса — `std::size_t`
  или другой, не полагайтесь на `int`.
- `mdspan` не владеет данными — базовый буфер должен быть жив.

## Задача

В `ex16_cpp23_syntax.cpp`:

1. напишите `parse_positive(int) -> std::expected<int, std::string>`;
2. обойдите вектор через `views::enumerate`;
3. соберите новый вектор через `ranges::to`;
4. сделайте `std::byteswap` для `std::uint32_t`;
5. посчитайте через `mdspan` элемент двумерной матрицы.

## Где применяется

- `expected` — парсеры, сетевые операции, валидаторы без исключений.
- `mdspan` — image/heightmap/grid-обработка (тайлы, аудио, нейросети).
- `byteswap` — сетевой порядок байт, чтение бинарных форматов.

## Проверка

Прогон печатает результат парсинга, пары индекс/значение, новый вектор, swapped
число и элемент матрицы; завершается `0`.

### Решение

См. `ex16_cpp23_syntax.cpp`.
