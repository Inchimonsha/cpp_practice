# Примеры th02 — типы, константы, условия, циклы

Все примеры собираются CMake (см. `CMakeLists.txt`) и вызываются из
корневого каталога курса:

```bash
cmake --build build --target th02_example_types_and_limits
cmake --build build --target th02_example_const_qualifiers
cmake --build build --target th02_example_loops_conditions
```

Либо вручную, из каталога темы:

```bash
g++ -std=c++20 -Wall -Wextra -o /tmp/th02_types examples/types_and_limits.cpp
```

- `types_and_limits.cpp` — размеры и диапазоны базовых типов
  (`sizeof`, `std::numeric_limits`); показывает, почему нельзя «зашивать»
  размеры в код.
- `const_qualifiers.cpp` — `const`/`constexpr`/`static` на практике:
  константы-настройки, счётчик вызовов, конфигурация «из рантайма».
- `loops_conditions.cpp` — условия, `switch`, циклы, `break`/`continue`;
  мини-практика из раздела 16 теории (`sum_even`).