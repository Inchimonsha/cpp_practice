# m00_tooling/ex04_cmake_multi — Многофайловый проект и CMake

> **Модуль:** `m00_tooling` · **Сложность:** ★★☆☆☆ · **Время:** 40–60 мин
> **Сборка:** CMake (не `run.ps1`) — см. раздел «Проверка».

> Это единственное задание модуля, оформленное как отдельный проект с
> несколькими единицами трансляции. Оно показывает переход от single-file к
> реальной структуре `include/` + `src/`.

## Теория (кратко)

Почти любой реальный код состоит из заголовков (`.hpp` — объявления, то, что
видят другие модули) и исходников (`.cpp` — определения). Граница между ними —
это **интерфейс**. CMake описывает, как из этих файлов собираются цели
(`target`): библиотека и исполняемый файл, и как они линкуются.

| Понятие CMake | Смысл |
|---|---|
| `add_library(name ...)` | собрать библиотеку (здесь `STATIC`) |
| `add_executable(name ...)` | собрать исполняемый файл |
| `target_include_directories(... PUBLIC include)` | кому видны заголовки |
| `target_link_libraries(app PRIVATE mathx)` | связать exe с библиотекой |
| `PUBLIC` / `PRIVATE` | «передаётся потребителям» / «только внутри» |

## Синтаксис

```cpp
// include/mathx.hpp — объявления (интерфейс)
#pragma once
namespace mathx {
[[nodiscard]] long long gcd(long long a, long long b);
}
```

```cpp
// src/mathx.cpp — определения (реализация)
#include "mathx.hpp"
long long mathx::gcd(long long a, long long b) { /* ... */ }
```

```cmake
add_library(mathx STATIC src/mathx.cpp)
target_include_directories(mathx PUBLIC include)
add_executable(ex04_cmake_multi src/main.cpp)
target_link_libraries(ex04_cmake_multi PRIVATE mathx)
```

## Подводные камни

- `#include "mathx.hpp"` ищется относительно каталога **потребителя**; каталог
  `include/` подключается через `target_include_directories`.
- `PUBLIC`-заголовки обязаны быть самодостаточными (включать `<cstdint>`, а не
  полагаться на порядок включения).
- Без `CMAKE_CXX_STANDARD_REQUIRED ON` компилятор может откатиться на старый
  стандарт молча.

## Задача

Доведите проект так, чтобы он собирался и печатал результат:

1. реализуйте `mathx::add`, `mathx::mul`, `mathx::gcd` в `src/mathx.cpp`;
2. `src/main.cpp` печатает `gcd(1071, 462)`;
3. сборка идёт без предупреждений (`-Wall -Wextra -Wpedantic`);
4. заголовок самодостаточен (компилируется отдельно).

## Где применяется

- Разделение «библиотека + приложение» — база любого движка: `engine` (static)
  линкуется в `game` (exe) и в тесты.
- `PUBLIC`/`PRIVATE` отражают реальные зависимости: то, что протекло наружу
  случайно, потом невозможно изменить без ломающих изменений.
- `install()`/`export()` (модуль `m08_build`) позволяют другим проектам
  подключать вашу библиотеку через `find_package`.

## Проверка

```powershell
cd advanced/m00_tooling/ex04_cmake_multi
cmake -S . -B build
cmake --build build
./build/ex04_cmake_multi.exe
```

Ожидаемый вывод: `gcd(1071, 462) = 21`.

### Решение

См. `ex04_cmake_multi/` — готовый проект.
