# m14_quality/ex03_coverage — покрытие кода (line/function/branch)

> Модуль m14_quality · Сложность ★★ · C++23 · Запуск: `./run.ps1 m14_quality/ex03_coverage`
>
> Инструментирование в примере — **ручное** (детерминировано). Реальный `gcov`
> проверен на этом тулчейне: `gcov 16.2.0`.

## Теория

**Покрытие кода** показывает, какая доля кода исполнена тестами:

- **line coverage** — выполненные строки;
- **function coverage** — вызванные функции;
- **branch coverage** — пройденные ветви условий. Именно она чувствительна к
  непроверенным `else`/`case`: 100% строк может соседствовать с ~50% ветвей.

Инструментарий компилятора (`--coverage`) вставляет счётчики; после запуска
тестов утилита агрегирует данные в отчёт. Важно: **высокое покрытие ≠
корректность** — оно лишь показывает непроверенный код.

## Синтаксис

Ручная модель (учебная): цель отмечает вход в функции и взятые ветви.

```cpp
struct Coverage {
    std::map<std::string, bool> branch_hits;
    std::map<std::string, bool> function_hits;
};

int grade(int score, Coverage& coverage) {
    coverage.function_hits["grade"] = true;
    score = clamp_score(score, coverage);
    if (score == 0)   { coverage.branch_hits["negative"] = true; return -1; }
    if (score < 50)   { coverage.branch_hits["fail"] = true; return 2; }
    if (score < 75)   { coverage.branch_hits["pass"] = true; return 3; }
    coverage.branch_hits["excellent"] = true; return 5;
}
```

Реальный `gcov` (проверено на MSYS2 ucrt64, GCC 16.2, `gcov 16.2.0`):

```bash
g++ -std=c++23 --coverage -O0 ex03_coverage.cpp -o cov.exe
./cov.exe
gcov -b -o . ex03_coverage.cpp      # -b = pokazat' vetvi
```

Пример вывода `gcov` для аналогичной цели:
`Lines executed:88.89% of 9`, `Branches executed:100.00% of 4`,
`Taken at least once:75.00% of 4`.

## Подводные камни

- **Оптимизация искажает покрытие**: `-O2` сворачивает/переставляет ветви —
  измеряйте с `-O0`, иначе отчёт не соответствует источнику.
- **Несовпадение версий**: `gcov` должен быть той же версии, что компилятор
  (`gcov 16.2.0` ↔ `g++ 16.2.0`); иначе ошибка чтения `.gcno`.
- **Остаточные файлы**: `.gcda` накапливаются между запусками — очищайте перед
  измерением, иначе счётчики складываются.
- **100% покрытие не значит отсутствие багов**: не проверены сами ожидания
  (ассерты должны быть корректны).
- **Ветви в шаблонах/инлайне** размножаются — большие заголовки могут искажать
  общую картину.
- На Windows путь `gcov -o` указывайте в каталог артефактов.

## Задача

Инструментировать функции `clamp_score`/`grade`, прогнать три теста
(`-1`, `30`, `60`), посчитать function- и branch-покрытие и вывести
непокрытые ветви. Ожидается: функции 100%, ветви 75% (`excellent` не покрыта).

## Где применяется

- Контроль качества тестов в CI, поиск мёртвого кода, отчётность по требованиям
  (например, авионика/медицина требует branch coverage).

## Проверка

Вывод: `uncovered branches: excellent`,
`function coverage = 100% (2/2)`, `branch coverage = 75% (3/4)`,
`coverage done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex03_coverage.cpp`. Реальные инструменты: **gcov/lcov** (GCC),
**llvm-cov** (Clang), **OpenCppCoverage** (MSVC), **Codecov** (сервис).
