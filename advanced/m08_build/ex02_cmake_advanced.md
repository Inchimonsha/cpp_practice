# ex02_cmake_advanced — цели, `INTERFACE`, установка

## Теория

Современный CMake строится вокруг **целей** (targets) и их **свойств**, а не
глобальных переменных. Ключевые понятия:

- `add_library(name STATIC|SHARED|INTERFACE|MODULE ...)`, `add_executable`.
- **Области видимости** связывания: `PUBLIC` (и самому, и потребителям),
  `PRIVATE` (только себе), `INTERFACE` (только потребителям).
- `INTERFACE`-библиотека не имеет своих исходников: она несёт только
  использование-требования (флаги, include-пути) и удобна для общих настроек.
- `target_include_directories`/`target_compile_options`/`target_link_libraries`
  работают по цели и распространяются транзитивно.
- **Install/export**: `install(TARGETS ...)` + `install(EXPORT)` позволяют
  другим проектам подключаться через `find_package`.

## Синтаксис

```cmake
add_library(cc_warnings INTERFACE)
target_compile_options(cc_warnings INTERFACE -Wall -Wextra)

add_library(shapes STATIC src/shape.cpp)
target_include_directories(shapes PUBLIC include)
target_link_libraries(shapes PRIVATE cc_warnings)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE shapes cc_warnings)
```

## Подводные камни

- Глобальные `include_directories()`/`CMAKE_CXX_FLAGS` протекают на всё и
  ломают изоляцию — предпочитайте `target_*`.
- `PRIVATE` vs `PUBLIC` у заголовочных зависимостей: если публичный заголовок
  включает чужой заголовок, нужен `PUBLIC`, иначе — ошибки у потребителей.
- Отсутствие `cmake_minimum_required`/`project` даёт неопределённое поведение.
- Не хардкодьте пути компилятора: задавайте toolchain/`CMAKE_CXX_COMPILER`.
- `install(EXPORT)` требует namespace и конфиг-файла — легко забыть.

## Задача

Собрать проект из трёх целей: `INTERFACE`-библиотека `cc_warnings` с флагами,
статическая библиотека `shapes` (`.hpp`+`.cpp`) и исполняемый `app`. Проверить
сборку через CMake. `app` печатает площади круга и прямоугольника; код возврата
`0`, если площадь прямоугольника `2x3` равна `6`.

## Где применяется

- Поставка библиотек и SDK, повторно используемые наборы настроек, интеграция
  сторонних проектов через `find_package`.

## Проверка

```text
cmake -S advanced/m08_build/ex02_cmake_advanced -B advanced/m08_build/ex02_cmake_advanced/build
cmake --build advanced/m08_build/ex02_cmake_advanced/build
advanced/m08_build/ex02_cmake_advanced/build/app.exe
```

Вывод: `circle r=1: 3.14`, `rect 2x3: 6.00`; код возврата `0`.

## Решение

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(cmake_advanced LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cc_warnings INTERFACE)
target_compile_options(cc_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow)

add_library(shapes STATIC src/shape.cpp)
target_include_directories(shapes PUBLIC include)
target_link_libraries(shapes PRIVATE cc_warnings)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE shapes cc_warnings)

include(GNUInstallDirs)
install(TARGETS shapes app
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
```

`include/shape.hpp`:

```cpp
#pragma once

namespace shapes {
double area_circle(double radius);
double area_rect(double width, double height);
}  // namespace shapes
```

`src/shape.cpp`:

```cpp
#include "shape.hpp"

namespace shapes {
double area_circle(double radius) {
    constexpr double kPi = 3.14159265358979323846;
    return kPi * radius * radius;
}

double area_rect(double width, double height) {
    return width * height;
}
}  // namespace shapes
```

`src/main.cpp`:

```cpp
#include <iomanip>
#include <iostream>

#include "shape.hpp"

int main() {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "circle r=1: " << shapes::area_circle(1.0) << "\n";
    std::cout << "rect 2x3: " << shapes::area_rect(2.0, 3.0) << "\n";
    return shapes::area_rect(2.0, 3.0) == 6.0 ? 0 : 1;
}
```
