# m04_stl/ex09_chrono — `std::chrono`

> **Модуль:** `m04_stl` · **Сложность:** ★★★☆☆ · **Время:** 40–60 мин
> **Запуск:** `./run.ps1 m04_stl/ex09_chrono`

## Теория (кратко)

`<chrono>` разделяет **длительность** (`duration`) и **момент времени**
(`time_point`). У каждого `clock` свой смысл: `system_clock` — настенные часы,
`steady_clock` — монотонные (для измерений). C++20 добавил **календарь**
(`year_month_day`, `weekday`) и дни/недели как единицы.

| Сущность | Смысл |
|---|---|
| `duration<Rep, Period>` | интервал времени |
| `time_point<Clock, Duration>` | точка на временной шкале |
| `steady_clock` | монотонные часы (тайминги) |
| `year_month_day`, `weekday` | календарные даты (C++20) |
| `duration_cast` | приведение периода |

## Синтаксис

```cpp
using namespace std::chrono;
const milliseconds total = 1500ms + 500ms;
const seconds sec = duration_cast<seconds>(total);   // 2s

const auto start = steady_clock::now();
do_work();
const auto elapsed = steady_clock::now() - start;    // duration

const year_month_day date{year{2026}, March, day{12}};
const weekday wd{sys_days{date}};
```

## Подводные камни

- Для **измерений** используйте `steady_clock`, не `system_clock` (последние
  могут «прыгнуть» при синхронизации времени).
- `duration_cast` **усекает** к меньшей точности (потеря остатка при `->`).
- Литералы `ms`, `s`, `h` требуют `using namespace std::chrono_literals` (или
  `std::chrono_literals`).
- `system_clock` — не единственный источник «истинного» времени; для игрового
  времени заводите собственный счётчик/`duration`.

## Задача

В `ex09_chrono.cpp`:

1. сложите `milliseconds` и приведите к секундам через `duration_cast`;
2. измерьте работу участка через `steady_clock` (выведите факт `>= 0`);
3. постройте `year_month_day` и выведите год/месяц/день;
4. получите `weekday` из даты;
5. заведите `duration` с нестандартным периодом и приведите его к `seconds`.

## Где применяется

- Профайлинг и бюджеты кадра (см. `m07_performance`).
- Таймеры, кулдауны, планирование задач.
- Работа с календарными датами (логи, снапшоты, расписания).

## Проверка

Прогон печатает вычисленные интервалы, дату и день недели; завершается `0`.

### Решение

См. `ex09_chrono.cpp`.
