# Sources: th05 — массивы и контейнеры

## Обязательные

- cppreference: [std::array](https://en.cppreference.com/w/cpp/container/array),
  [std::vector](https://en.cppreference.com/w/cpp/container/vector),
  [Iterators](https://en.cppreference.com/w/cpp/iterator),
  [std::span](https://en.cppreference.com/w/cpp/container/span).
- [learncpp.com, chapters 15–16](https://www.learncpp.com/) — арrays, итераторы,
  размер и ёмкость.

## Рекомендуемые

- cppreference: [Range-based for loop](https://en.cppreference.com/w/cpp/language/range-for)
  — как устроен сахар.
- S. Meyers, Effective Modern C++: Item 13 (`const_iterator` vs `iterator`),
  Item 14 (noexcept и reserve).

## Дополнительные

- Статья «[vector capacity behavior](https://en.cppreference.com/w/cpp/container/vector#Capacity)» —
  формальные инварианты `size`/`capacity`.
- Концепция «универсальная ссылка для span» — в th11; здесь достаточно
  «окна» над `const`.

## Требования курса

- Примеры — C++20, компилятор GCC (MSYS2 `ucrt64`); индексы —
  `std::size_t`; предупреждения — `-Wall -Wextra`.