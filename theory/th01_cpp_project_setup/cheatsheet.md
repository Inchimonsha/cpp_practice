# Шпаргалка: Настройка проекта

Быстрый справочник по теме `th01`. Развёрнутые объяснения — в
[theory.md](theory.md).

## Быстрый старт (команды)

```bash
# зависимости курса (один раз)
python tools/fetch_deps.py

# сборка и тесты
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure

# сборка только одной цели
cmake --build build --target hello

# Release
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

# валидаторы курса
python tools/validators/check_structure.py
python tools/validators/check_links.py
```

## Сборка C++-файла вручную (контроль, без CMake)

| Действие | GCC/Clang | MSVC |
|---|---|---|
| собрать | `g++ -std=c++20 main.cpp -o app.exe` | `cl /EHsc main.cpp` |
| + предупреждения | `-Wall -Wextra` | `/W4` |
| Debug | `-O0 -g` | `/Od /Zi` |
| Release | `-O2 -DNDEBUG` | `/O2 /DNDEBUG` |

## Минимальный CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(app VERSION 0.1.0 LANGUAGES CXX)

add_executable(app main.cpp)
# при необходимости:
# target_include_directories(app PRIVATE include)
# target_link_libraries(app PRIVATE cpp_course_common)
```

## Git

```bash
git init
git add .                      # либо выборочно: git add main.cpp
git commit -m "feat: ..."
git status && git log --oneline
git checkout -b feature/x      # создать и переключиться
git push -u origin main
git pull
git clone <url>
```

Формат сообщений: `feat:`, `fix:`, `docs:`, `test:`, `refactor:`, `chore:`.

## Типы ошибок

| Ошибка | Признак | Где искать |
|---|---|---|
| компиляции | упоминается `.cpp`/`.hpp` и строка, `error:` | синтаксис/типы |
| линковки | `undefined reference`, `unresolved external` | не прилинкована библиотека/файл |
| рантайм | `segmentation fault`, исключение, зависание | логи, отладчик |

## Полезные макросы (C++20)

```cpp
__cplusplus            // 202002L для C++20
__FILE__, __LINE__     // место в исходнике
__GNUC__, __clang__, _MSC_VER  // какой компилятор
#ifdef NDEBUG          // Release-сборка
```

## Чек-лист

- [ ] проект собирается в Debug и Release
- [ ] `.gitignore` закрывает `build/`, `third_party/`, артефакты
- [ ] коммиты осмысленные, один коммит — одна правка
- [ ] пути к ресурсам относительны исполняемого файла
- [ ] предупреждения включены (`-Wall -Wextra` или `/W4`)
- [ ] лицензии ассетов задокументированы