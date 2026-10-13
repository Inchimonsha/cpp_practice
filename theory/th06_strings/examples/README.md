# Примеры th06 — строки

```bash
cmake --build build --target th06_example_strings_basics
cmake --build build --target th06_example_string_view
cmake --build build --target th06_example_encoding_notes
```

- `strings_basics.cpp` — конкатенация, поиск, вырезка, числа.
- `string_view.cpp` — чтение без копий и ограничение времени жизни.
- `encoding_notes.cpp` — байты vs символы в UTF-8, ASCII-вывод.