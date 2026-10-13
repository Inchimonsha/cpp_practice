# m10_language_future/ex06_comptime_meta — метапрограммирование времени компиляции

## Теория

Современный C++ позволяет вычислять **на этапе компиляции** почти всё, что не
требует ввода-вывода:

- `constexpr` — функция может исполняться и во время компиляции, и в рантайме.
- `consteval` — функция **обязана** вычисляться во время компиляции (immediate
  function); ошибка, если её нельзя вычислить статически.
- `if consteval` — ветвление в зависимости от того, вычисляется ли функция
  статически; заменяет `std::is_constant_evaluated()` и удобнее в `if constexpr`
  -контекстах.
- `static_assert` — проверка инвариантов на этапе компиляции.

Вместе с `consteval`-генерацией данных (таблиц, хешей, простых чисел) это
устраняет рантайм-стоимость и даёт проверяемость на компиляции.

## Синтаксис

```cpp
consteval std::uint64_t compile_time_hash(std::string_view s) { return fnv1a(s); }

template <std::size_t N>
consteval std::array<int, N> first_primes() { /* ... */ }

constexpr int evaluate(int x) {
    if consteval { return x * 2; }       // staticheskaya vetv'
    else        { return x * 3; }        // dinamicheskaya vetv'
}

static_assert(first_primes<8>()[7] == 19);
```

## Подводные камни

- `consteval`-функция в рантайме — ошибка компиляции; `constexpr`-функция может
  «деградировать» до рантайма.
- `if consteval` выбирается по **контексту вызова**, а не по оптимизации:
  `const int v = evaluate(5);` всё ещё константное выражение, поэтому берётся
  статическая ветвь. Чтобы получить рантайм-ветвь, нужен **не** константный
  аргумент (например, из `volatile`).
- Хеш-таблица «по FNV-1a» использует хеш как ключ `switch`; теоретически
  возможны коллизии — для корректности нужна дополнительная проверка строки.
- `static_assert` требует **константного** выражения; вызовы функций ввода-вывода
  внутри `consteval`-функций невозможны.
- `std::array`, возвращаемый из `consteval`, полностью размещается на стеке — не
  делайте таблицы гигантскими.

## Задача

Реализовать: `constexpr`-хеш FNV-1a и `consteval`-обёртку; `consteval`
`is_prime` + `first_primes<N>()`; `constexpr` `evaluate` с `if consteval`;
`color_of` — разбор имени цвета через `switch` по хешу. Закрепить инварианты
`static_assert` и продемонстрировать разницу статической/динамической ветви.

## Где применяется

- Генерация таблиц (тригонометрия, CRC, шрифты), строковые ключи без аллокаций,
  compile-time парсинг конфигураций, проверки формата на этапе сборки, DSL.

## Проверка

Вывод: `fnv1a("send")`, `first_primes<8> = 2 3 5 7 11 13 17 19`,
`color_of("green") = 0xff00`, `color_of("magenta") = -1`,
`evaluate compile-time = 10, runtime = 15`. Код возврата `0` при совпадении,
иначе `1`. Запуск с `-Std c++26`.

## Решение

См. `ex06_comptime_meta.cpp`. Ключевые фрагменты:

```cpp
constexpr std::uint64_t fnv1a(std::string_view text) {
    std::uint64_t hash = 1469598103934665603ull;
    for (const char ch : text) {
        hash ^= static_cast<unsigned char>(ch);
        hash *= 1099511628211ull;
    }
    return hash;
}

consteval bool is_prime(const int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; ++d)
        if (n % d == 0) return false;
    return true;
}

template <std::size_t N>
consteval std::array<int, N> first_primes() {
    std::array<int, N> result{};
    int candidate = 2;
    std::size_t found = 0;
    while (found < N) {
        if (is_prime(candidate)) result[found++] = candidate;
        ++candidate;
    }
    return result;
}

constexpr int evaluate(const int x) {
    if consteval { return x * 2; }
    else         { return x * 3; }
}
```
