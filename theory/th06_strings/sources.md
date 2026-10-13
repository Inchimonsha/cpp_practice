# Sources: th06 — строки

## Обязательные

- cppreference: [std::string](https://en.cppreference.com/w/cpp/string/basic_string),
  [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view),
  [std::to_string](https://en.cppreference.com/w/cpp/string/basic_string/to_string),
  [std::stoi](https://en.cppreference.com/w/cpp/string/basic_string/stol).
- [learncpp.com, chapters 17][`learncpp.com strings`](https://www.learncpp.com/cpp-tutorial/an-introduction-to-stdstring/)

## Рекомендуемые

- cppreference: [Range of UTF-8](https://en.cppreference.com/w/cpp/language/types#Character_types)
  — про `char`/`char8_t` и UTF-8.
- Руководство по строкам Google C++ Style Guide (раздел про ownership).

## Дополнительные

- «[The Absolute Minimum Every Software Developer Absolutely, Positively Must
  Know About Unicode](https://www.joelonsoftware.com/2003/10/08/)» — Joel
  Spolsky о кодировках простым языком.
- Сайт International Components for Unicode (ICU) — если понадобится
  нормализация строк (позже, в th33).

## Требования курса

- Примеры — C++20; логи курса — ASCII; параметры «чтение текста» —
  `std::string_view`.