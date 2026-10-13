# Источники: Настройка проекта

## Официальная документация

- **C++**: [cppreference.com](https://en.cppreference.com/w/) — основной
  справочник; раздел [C++ language](https://en.cppreference.com/w/cpp/language).
- **CMake**: [документация CMake](https://cmake.org/cmake/help/latest/) — справочник
  команд/переменных.
- **Git**: [git-scm.com/doc](https://git-scm.com/doc) — руководства и справочник
  команд; [Pro Git](https://git-scm.com/book/ru/v2) (русский перевод книги).
- **SFML**: [документация 3.1.0](https://www.sfml-dev.org/documentation/) и
  [tutorials](https://www.sfml-dev.org/tutorials/).
- **Lua**: [справочник Lua 5.4](https://www.lua.org/manual/5.4/).

## Стандарт C++

- [Working Draft / стандарт C++20](https://en.cppreference.com/w/cpp/20) —
  сводка новинок стандарта.
- Статья «[Sixteen ways to call a function](https://blog.coldflake.com/posts/C++-17-virtual-polymorphism/)»
  (опционально): наглядный пример, почему ABI-совместимость — сложная тема.

## Окружение курса (проверено)

- ОС: Windows; компилятор **MinGW-w64 UCRT GCC 16.2.0** (MSYS2 `ucrt64`).
- **CMake 3.31.0**; сборка через `cmake -S . -B build`.
- **SFML 3.1.0** пребилт `windows-gcc-14.2.0-mingw-64-bit` (winlibs, UCRT).
- **Lua 5.4.9** — сборка из исходников (FetchContent).
- Python 3.13 для скриптов и валидаторов.

> **Почему не «любой» GCC.** Пребилт собран под UCRT. msvcrt-тулчейн (например,
> bare GCC 13.2) слинкует приложение и даже запустит логику без графики, но
> оконное SFML-приложение упадёт при запуске из-за кросс-CRT heap-коррупции
> (`0xC0000005`, диагностика — в `pitfalls.md`). Поэтому SFML-цели требуют
> UCRT-тулчейна (MSYS2 `ucrt64` / winlibs UCRT).

## Как проверялась тема

- Примеры `hello.cpp`, `diagnostics.cpp`, `build_info.cpp` скомпилированы
  MinGW UCRT-тулчейном (`-std=c++20`) и запущены.
- Команды сборки курса выполнены: `cmake -S . -B build`, `cmake --build build`,
  `ctest --test-dir build`.
- Оконные приложения `ex01_solution` и `prj01_arcade` запущены из `build/`
  (работают без до-настройки: POST_BUILD копирует DLL пребилта и рантайм
  компилятора).

## Материалы по лицензированию ассетов

- [choosealicense.com](https://choosealicense.com/) — выбор открытой лицензии.
- CC0 / CC-BY: [creativecommons.org](https://creativecommons.org/).
- Пройдёт отдельный разбор в `th51_ci_release`.

## Практические материалы курса

- Задание: ex01 — Hello, Game Window.
- Проект: prj01 — Arcade Game.