# Sources: th02 — типы, операторы, условия, циклы

## Обязательные

- cppreference: [Fundamental types](https://en.cppreference.com/w/cpp/language/types),
  [Operators](https://en.cppreference.com/w/cpp/language/operator_precedence),
  [constexpr](https://en.cppreference.com/w/cpp/language/constexpr).
- [learncpp.com, chapters 1–4](https://www.learncpp.com/) — переменные, типы,
  константы, операторы (`std::print` там заменяйте на `std::cout`).

## Рекомендуемые

- B. Stroustrup, «Программирование. Принципы и практика с использованием C++»
  (главы 2–8).
- S. Meyers, Effective Modern C++: Item 7 (brace-инициализация), Item 8
  (`nullptr`).
- Standford CS106L notes: «Types and variables».

## Дополнительные

- Статья «[What every computer scientist should know about floating-point
  arithmetic](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)»
  (Goldberg, 1991) — почему `0.1 + 0.2 != 0.3`.
- cppreference: [Implicit conversions](https://en.cppreference.com/w/cpp/language/implicit_conversion)
  — полная таблица правил сужения/расширения.

## Требования курса

- Все примеры — C++20, компилятор GCC (MSYS2 `ucrt64`); окружение — см.
  `th01/sources.md`.
- Для печати используется `<iostream>` (`std::print` в GCC < 14 недоступен).