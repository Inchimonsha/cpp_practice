# m03_templates/ex06_compile_time — Compile-time вычисления

> **Модуль:** `m03_templates` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m03_templates/ex06_compile_time`

## Теория (кратко)

`constexpr`-функции вычисляются и в рантайме, и в compile-time; `consteval`
(**immediate functions**) обязаны вычисляться только в compile-time;
`if constexpr` отбрасывает неподходящие ветки до инстанцирования;
`static_assert` проверяет условия на этапе компиляции.

| Инструмент | Где вычисляется | Ошибка, если не compile-time |
|---|---|---|
| `constexpr` fn | там, где возможно | нет |
| `consteval` fn | только compile-time | да |
| `constinit` var | инициализация статически | — |
| `if constexpr` | отбор ветки | — |
| `static_assert` | проверка условия | да, при `false` |

## Синтаксис

```cpp
constexpr int fib(int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }
static_assert(fib(10) == 55);

consteval int square(int x) { return x * x; }   // tol'ko compile-time

template <class T>
std::string kind() {
    if constexpr (std::is_integral_v<T>) { return "int"; }
    else { return "other"; }
}
```

## Подводные камни

- `constexpr`-функция «годится» и в рантайме — не путайте с `consteval`.
- Рекурсия в compile-time ограничена глубиной инстанцирования компилятора.
- `if constexpr` не заменяет `if`: в отброшенной ветке код всё равно должен
  быть синтаксически корректным для **независимых** имён, но не инстанциируется
  для зависимых.
- Тяжёлые вычисления на этапе компиляции замедляют сборку.

## Задача

В `ex06_compile_time.cpp`:

1. `constexpr fib(int)` и `static_assert` на результат;
2. `consteval square(int)`; вызовите в `constexpr`-контексте;
3. таблицу значений (массив) посчитайте в compile-time;
4. `if constexpr`-функцию `kind<T>()`;
5. `constinit`-переменную и `constexpr`-константу.

## Где применяется

- Таблицы синусов/степеней, look-up таблицы без рантайм-вычислений.
- Проверки инвариантов (`static_assert` на `sizeof`, на формат).
- Выбор реализации по типу без накладных расходов (`if constexpr`).

## Проверка

Прогон печатает compile-time таблицу и значения; завершается `0`.

### Решение

См. `ex06_compile_time.cpp`.
