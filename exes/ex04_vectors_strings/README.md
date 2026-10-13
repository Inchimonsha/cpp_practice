# ex04 — Event Log: журнал игровых событий

**Закрепляет:** `std::vector` и `std::string`/`std::string_view` (th05, th06).

| | |
|---|---|
| Сложность | ★★★☆☆ |
| Время | 2–4 часа |
| Знания | th01–th06 |
| Связанные темы | [th05 — Массивы и контейнеры](../../theory/th05_arrays_containers/), [th06 — Строки](../../theory/th06_strings/) |
| Связанный проект | [prj01 — Arcade Game](../../prjs/prj01_arcade_game/) |
| Требует | только C++20 (без SFML) |

## Что будет в решении

- журнал игровых событий: добавление, поиск, фильтрация, статистика;
- мини-REPL (консольная программа) с командами `add`, `count`, `filter`,
  `stats`, `quit`;
- разбор команд без лишних копий — через `std::string_view`;
- логика в `shared/`, покрыта unit-тестами.

## Структура

```
ex04_vectors_strings/
├── task.md            ← постановка
├── shared/            ← логика журнала (тестируемая)
├── starter/           ← каркас с TODO (REPL с пропущенными командами)
├── solution/          ← эталонное решение
├── tests/             ← unit-тесты
├── hints.md           ← подсказки
└── review.md          ← разбор и критерии
```

## Сборка и запуск

```bash
cmake -S . -B build
cmake --build build --target ex04_solution
./build/exes/ex04/ex04_solution.exe       # интерактивный журнал
ctest --test-dir build --output-on-failure -R ex04
```

Пример сессии:

```
> add Kill Monster
> add Pickup Coin
> count !          → 1
> filter Kill      → Kill Monster
> stats            → событий: 2, средняя длина: 11.5
> quit
```