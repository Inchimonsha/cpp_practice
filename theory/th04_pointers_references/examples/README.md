# Примеры th04 — ссылки, указатели, стек и куча

```bash
cmake --build build --target th04_example_references
cmake --build build --target th04_example_pointers
cmake --build build --target th04_example_stack_heap
```

- `references.cpp` — ссылки, `const&`, возврат ссылки, время жизни.
- `pointers.cpp` — `nullptr`, перепривязка, проверка перед разыменованием.
- `stack_heap.cpp` — стек vs куча; демонстрация `new`/`delete` (и почему мы
  в курсе их избегаем) и `std::unique_ptr` как правильная замена.