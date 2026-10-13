# prj01 — Arcade Game

**Жанр:** 2D-аркада (тип «catch/dodge + накопление очков»).

Первый пет-проект курса. Объединяет модули 0–4: окно и игровой цикл (SFML),
векторы и коллизии (математика), структура проекта и Git.

| | |
|---|---|
| Образовательная цель | цельный игровой цикл «ввод → симуляция → рендер», тесты, сохранение рекорда |
| Связанная теория | th01, th19–th22 (по мере блоков), th23 |
| Связанное задание | [ex01](../../exes/ex01_hello_game_window/) |
| Стадия | каркас (Vertical slice — ближайшая веха) |

## Команды

```bash
python tools/fetch_deps.py                      # один раз
cmake -S . -B build
cmake --build build --target prj01_arcade
./build/prjs/prj01/prj01_arcade.exe

# тесты проекта
ctest --test-dir build --output-on-failure -R prj01
```

## Документация проекта

| Файл | Содержание |
|---|---|
| [brief.md](brief.md) | постановка: жанр, цель, проблема, что изучается |
| [requirements.md](requirements.md) | требования, MVP, vertical slice, инструменты |
| [milestones.md](milestones.md) | вехи и декомпозиция задач |
| [architecture.md](architecture.md) | архитектура, подсистемы, модель данных, жизненный цикл |
| [design/](design/) | геймдизайн: концепт, экраны, список ассетов |
| [docs/](docs/) | стратегии тестирования/отладки/профилирования/сохранений, code review |
| [CHANGELOG.md](CHANGELOG.md) | журнал изменений проекта |

## Структура кода

```
prj01_arcade_game/
├── include/arcade/   ← публичные заголовки (score.hpp, entity.hpp, game.hpp)
├── src/              ← реализация (main.cpp, game.cpp, score.cpp, entity.cpp)
├── tests/            ← unit-тесты логики (без окна)
└── assets/           ← ассеты проекта (шрифт добавляется на этапе UI)
```

Логика (`Score`, `Aabb`, `Entity`) не зависит от SFML и тестируется без окна;
`Game` (окно + цикл) — тонкий связующий слой. Это принцип разделения
симуляции и визуализации (th34) в действии с первого проекта.