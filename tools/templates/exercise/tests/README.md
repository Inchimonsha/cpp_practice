# tests/

Тесты задания. Запуск против решения:

```bash
cmake -S . -B build -DCPP_COURSE_ENABLE_SFML=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Тесты проверяют контракт, а не внутренности решения.