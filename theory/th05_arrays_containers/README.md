# th05 — Массивы и контейнеры: std::array, std::vector, итераторы

**Модуль 1. Основы C++.**

Как хранить наборы объектов: от фиксированных массивов до растущего вектора и
безопасных окон `std::span`.

| | |
|---|---|
| Цели | выбирать `std::array`/`std::vector` под задачу, использовать итераторы, `reserve`, `emplace_back`, `std::span` |
| Пред. знания | [th02](../th02_cpp_basics_syntax/), [th03](../th03_functions_scope/), [th04](../th04_pointers_references/) |
| Связанные задания | [ex04 — Event Log](../../exes/ex04_vectors_strings/) |
| Связанные проекты | [prj01 — Arcade Game](../../prjs/prj01_arcade_game/) |
| Статус | готова |

## Содержание

- [theory.md](theory.md) — полный обучающий материал
- [cheatsheet.md](cheatsheet.md) — шпаргалка
- [pitfalls.md](pitfalls.md) — типичные ошибки и подводные камни
- [sources.md](sources.md) — источники
- [examples/](examples/) — компилируемые примеры

## Основное одной фразой

`std::vector` — «массив, который умеет расти», `std::array` — «массив
фиксированного размера на стеке»; итераторы и `std::span` позволяют писать
код, которому всё равно, откуда взялись элементы.