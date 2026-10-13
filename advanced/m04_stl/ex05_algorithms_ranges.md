# m04_stl/ex05_algorithms_ranges — Алгоритмы и `std::ranges`

> **Модуль:** `m04_stl` · **Сложность:** ★★★★☆ · **Время:** 60–80 мин
> **Запуск:** `./run.ps1 m04_stl/ex05_algorithms_ranges`

## Теория (кратко)

`<algorithm>` и `<numeric>` дают десятки готовых алгоритмов. Версии из
`std::ranges` принимают диапазон целиком (без пары `begin/end`) и поддерживают
**проекции** — функцию, применяемую к элементу до сравнения.

| Классика | ranges |
|---|---|
| `std::sort(v.begin(), v.end())` | `std::ranges::sort(v)` |
| `std::find(v.begin(), v.end(), x)` | `std::ranges::find(v, x)` |
| `std::accumulate(...)` | `std::ranges::fold_left(...)` (C++23) |
| — | проекция: `ranges::sort(v, {}, &T::field)` |

## Синтаксис

```cpp
std::ranges::sort(people, std::greater{}, &Person::score);   // proekciya

auto it = std::ranges::find(people, "bob", &Person::name);

std::vector<std::string> names;
std::ranges::transform(people, std::back_inserter(names), &Person::name);

const int total = std::accumulate(scores.begin(), scores.end(), 0);
const bool anyTop = std::ranges::any_of(people, [](const Person& p){ return p.score > 90; });
```

## Подводные камни

- Проекция меняет, **по какому полю** сравнивают/ищут, но не сам алгоритм.
- `std::accumulate` — из `<numeric>`; порядок свёртки слева-направо.
- `ranges::sort` не принимает временный rvalue-диапазон, который нельзя
  переставить (например, view — нужен `to< vector >`).
- Компаратор должен задавать строгий слабый порядок (`<`), не `<=`.

## Задача

В `ex05_algorithms_ranges.cpp`:

1. отсортируйте `Person` по `score` по убыванию через проекцию;
2. найдите человека по имени (`ranges::find` + проекция);
3. соберите имена через `ranges::transform` + `back_inserter`;
4. посчитайте сумму баллов (`std::accumulate`);
5. проверьте `ranges::any_of`/`all_of`/`count_if`.

## Где применяется

- Пакетная обработка сущностей, сортировка по ключу (рендер-очередь, AI).
- Агрегации (суммы/средние), фильтрация выборок.
- Проекции упрощают код: не пишем компараторы вручную.

## Проверка

Прогон печатает отсортированный список, найденное имя, сумму и предикаты;
завершается `0`.

### Решение

См. `ex05_algorithms_ranges.cpp`.
