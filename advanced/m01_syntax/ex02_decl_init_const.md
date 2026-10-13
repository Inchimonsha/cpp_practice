# m01_syntax/ex02_decl_init_const — Объявление, инициализация, const

> **Модуль:** `m01_syntax` · **Сложность:** ★★☆☆☆ · **Время:** 40–60 мин
> **Запуск:** `./run.ps1 m01_syntax/ex02_decl_init_const`

## Теория (кратко)

Почти все имена в C++ можно объявлять по-разному. Предпочтение курса —
**list-initialization `{}`**: она запрещает сужающие преобразования и не путается
с объявлением функции. `const`/`constexpr` выражают неизменяемость; `constexpr`
и `consteval` переносят вычисления на этап компиляции.

| Квалификатор | Смысл |
|---|---|
| `const` | значение не меняется после инициализации |
| `constexpr` | вычислимо на этапе компиляции (и `const`) |
| `consteval` | функция **обязана** вызываться на этапе компиляции |
| `constinit` | статическая инициализация (никаких «порядковых» сюрпризов) |
| `static` (локально) | хранит значение между вызовами |
| `inline` (переменная) | одно определение в разных TU |
| `thread_local` | отдельный экземпляр на поток |

## Синтаксис

```cpp
int a{};        // 0
int b{7};       // 7
int c(7);       // 7 (direct)
int d = 7;      // 7 (copy)
std::vector<int> v{1, 2, 3};   // initializer_list

constexpr int square(int x) { return x * x; }
consteval int cube(int x) { return x * x * x; }   // обязательный compile-time
constinit inline int g_seed = 12345;              // static init

struct Point { int x; int y; };
Point p{.x = 1, .y = 2};        // designated initializers (C++20)

int next_id() { static int id = 0; return ++id; } // static local
```

## Подводные камни

- `int x();` — это **объявление функции**, а не переменной. Используйте `int x{};`.
- `std::vector<int> v{5}` — вектор из одного элемента `5`, а не из пяти нулей
  (для этого `std::vector<int> v(5)` — «самый неприятный разбор»).
- `constexpr`-функция может вызываться и в рантайме; `consteval` — только в
  compile-time.
- Изменяемый `static` в многопоточном коде — источник гонок (нужна атомарность).

## Задача

В `ex02_decl_init_const.cpp`:

1. покажите все формы инициализации и их результат;
2. напишите `constexpr int square(int)` и `consteval int cube(int)`, проверьте
   `static_assert`;
3. заведите `constinit`-переменную и `static`-счётчик `next_id()`;
4. используйте designated initializer для `Point`;
5. продемонстрируйте разницу `std::vector<int> v{5}` vs `v(5)`.

## Где применяется

- `constexpr`-таблицы (sin/cos, CRC, размеры буферов) — ноль рантайм-стоимости.
- `constinit` спасает от «static initialization order fiasco» в глобальных
  реестрах (ресурсы, фабрики, логи).
- `{}`-инициализация — защита от неявных сужений в числовом коде.

## Проверка

Прогон печатает результаты инициализации и счётчики; завершается `0`.

### Решение

См. `ex02_decl_init_const.cpp`.
