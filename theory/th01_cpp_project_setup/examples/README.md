# Examples: th01 — Настройка проекта

Компилируемые примеры к теме. C++20.

| Пример | Что показывает |
|---|---|
| `hello.cpp` | минимальная программа «Hello, game!» |
| `diagnostics.cpp` | разница Debug и Release через `assert` и `NDEBUG` |
| `build_info.cpp` | версия C++, компилятор и платформа через макросы |

## Сборка

Через корневой CMake (рекомендуется):

```bash
cmake -S . -B build
cmake --build build --target th01_example_hello
cmake --build build --target th01_example_diagnostics
cmake --build build --target th01_example_build_info
```

Или вручную (проверка окружения):

```bash
g++ -std=c++20 hello.cpp -o hello.exe
g++ -std=c++20 -O0 -g diagnostics.cpp -o diagnostics_debug.exe
g++ -std=c++20 -O2 -DNDEBUG diagnostics.cpp -o diagnostics_release.exe
g++ -std=c++20 build_info.cpp -o build_info.exe
```

Результат: `build_info.exe` должен напечатать `__cplusplus` не меньше
`202002` — это подтверждает C++20.