# Sources: th04 — ссылки, указатели, управление памятью

## Обязательные

- cppreference: [References](https://en.cppreference.com/w/cpp/language/reference),
  [Pointers](https://en.cppreference.com/w/cpp/language/pointer),
  [Storage duration](https://en.cppreference.com/w/cpp/language/storage_duration),
  [new expressions](https://en.cppreference.com/w/cpp/language/new).
- [learncpp.com, chapters 9–11](https://www.learncpp.com/) — ссылки,
  указатели, динамическая память.

## Рекомендуемые

- S. Meyers, Effective C++: Item 16 (new/delete парами), Item 20
  (передача по const&). Effective Modern C++: Item 2 (auto и ссылки).
- R. Lafore, главы «Ссылки», «Указатели», «Динамическое выделение памяти».

## Дополнительные

- Статья «[What is memory?](https://www.gnu.org/software/bgne/faq/memory.html)»
  или аналоги вводных по модели памяти ОС (стек/куча/mmap).
- cppreference: [Object lifetime](https://en.cppreference.com/w/cpp/language/lifetime)
  — формальные правила начала и конца жизни объектов.

## Требования курса

- Примеры — C++20, без ручного `new`/`delete` (правило курса; в примере
  `stack_heap.cpp` демонстрация — только в защищённом виде).