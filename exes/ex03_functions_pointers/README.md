# ex03 — Squad Combat Simulator: функции, ссылки, указатели

**Закрепляет:** функции, области видимости, ссылки, указатели (th03, th04).

| | |
|---|---|
| Сложность | ★★☆☆☆ |
| Время | 1.5–3 часа |
| Знания | th01–th04 |
| Связанные темы | [th03 — Функции и области видимости](../../theory/th03_functions_scope/), [th04 — Ссылки и указатели](../../theory/th04_pointers_references/) |
| Связанный проект | [prj01 — Arcade Game](../../prjs/prj01_arcade_game/) |
| Требует | только C++20 (без SFML) |

## Что будет в решении

- отряд из трёх бойцов: лечим, бьём, находим самого слабого;
- ссылки — чтобы изменять бойца «по месту» (`Fighter&`);
- указатели — для «есть ли цель, а если нет — `nullptr`»;
- логика отряда в `shared/` и покрыта unit-тестами.

## Структура

```
ex03_functions_pointers/
├── task.md            ← постановка
├── shared/            ← логика отряда (тестируемая)
├── starter/           ← каркас с TODO
├── solution/          ← эталонное решение
├── tests/             ← unit-тесты
├── hints.md           ← подсказки
└── review.md          ← разбор и критерии
```

## Сборка и запуск

```bash
cmake -S . -B build
cmake --build build --target ex03_solution
./build/exes/ex03/ex03_solution.exe       # симуляция раунда
ctest --test-dir build --output-on-failure -R ex03
```