# m14_quality/ex06_ci_matrix — матрица сборки CI

> Модуль m14_quality · Сложность ★★ · C++23 · Запуск: `./run.ps1 m14_quality/ex06_ci_matrix`

## Теория

**CI (непрерывная интеграция)** прогоняет сборку и тесты на **матрице**
конфигураций: ОС × компилятор × стандарт × тип сборки. Цель — поймать
платформенные различия до релиза.

В GitHub Actions матрица задаётся:

```yaml
strategy:
  fail-fast: false
  matrix:
    os: [ubuntu-latest, windows-latest, macos-latest]
    compiler: [gcc-13, clang-17, msvc-19]
    standard: [c++20, c++23]
runs-on: ${{ matrix.os }}
```

Задание исполняется для каждой комбинации; `fail-fast: false` продолжает
остальные при падении одной. Пример здесь **моделирует** матрицу: правила
поддержки (какой компилятор на какой ОС существует) и один «известный
сломанный» случай, чтобы показать, как в отчёте выглядят pass/fail/skip.

## Синтаксис

```cpp
struct Job { std::string os, compiler, standard; };

bool supported(const Job& job);   // est' li takaya konfiguraciya
bool passes(const Job& job);      // uslovie "zelenogo" joba

for (const std::string& os : systems)
    for (const std::string& compiler : compilers)
        for (const std::string& standard : standards)
            jobs.push_back(Job{os, compiler, standard});
```

## Подводные камни

- **Комбинации «которых не бывает»** (например, MSVC на Linux) нужно исключать
  (`exclude`/`include`), иначе джобы вечно падают.
- **`fail-fast` по умолчанию `true`** — одна ошибка отменяет остальные; для
  диагностики ставьте `false`.
- **Разные имена инструментов**: не «gcc», а `g++-13`/`cc`; версии лучше
  пиновать, иначе течёт база сравнения.
- **Стоимость**: полная матрица тратит минуты CI — сужайте до значимых
  комбинаций (эталонная ОС + «канареечные» редкие).
- **Кэш зависимостей** обязателен, иначе каждая ячейка качает всё заново.
- **standard vs compiler**: не все комбинации «компилятор × стандарт»
  поддерживаются — проверяйте таблицу совместимости.

## Задача

Смоделировать матрицу `{ubuntu,windows,macos} × {gcc-13,clang-17,msvc-19} ×
{c++20,c++23}` (18 комбинаций): пропускать невозможные, отметить один
сломанный случай, вывести таблицу статусов, сводку и YAML-фрагмент матрицы.

## Где применяется

- Портативные C++-библиотеки, кросс-платформенные приложения, пакетные
  репозитории (vcpkg/Conan), релизная инфраструктура.

## Проверка

Вывод: строки `ci matrix (18 combinations)`, статусы `[run]`/`[skip]` для каждой
комбинации, `summary: passed = 11, failed = 1, skipped = 6`,
`strategy yaml:` и три строки матрицы, `ci matrix done`. Код возврата `0` при
ожидаемых счётчиках, иначе `1`.

## Решение

См. `ex06_ci_matrix.cpp`. Правила: MSVC — только Windows; GCC — не на macOS;
«сломанный» случай — `windows-latest / clang-17 / c++23`.
