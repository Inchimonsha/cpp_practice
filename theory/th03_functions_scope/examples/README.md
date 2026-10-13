# Примеры th03 — функции, перегрузка, области видимости

```bash
cmake --build build --target th03_example_functions_overloads
cmake --build build --target th03_example_scopes_static
cmake --build build --target th03_example_decl_vs_def
```

- `functions_overloads.cpp` — перегрузка, параметры по значению и по ссылке,
  `const&`.
- `scopes_static.cpp` — анонимный namespace, локальные переменные, `static`
  счётчик.
- `fraction.hpp` / `fraction.cpp` / `decl_vs_def.cpp` — разделение
  объявлений и определений на два файла: «живой» образец того, как собираются
  `shared/`-библиотеки заданий.