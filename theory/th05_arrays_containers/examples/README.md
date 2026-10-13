# Примеры th05 — массивы, контейнеры, итераторы

```bash
cmake --build build --target th05_example_array_vs_vector
cmake --build build --target th05_example_iterators
cmake --build build --target th05_example_span_window
```

- `array_vs_vector.cpp` — `std::array` для фиксированных таблиц; `std::vector`
  с `reserve` и `emplace_back`; разница `size`/`capacity`.
- `iterators.cpp` — range-for, изменение элементов, `at()` vs `[]`.
- `span_window.cpp` — единая функция на `std::span` для `array`, `vector` и
  C-массива.