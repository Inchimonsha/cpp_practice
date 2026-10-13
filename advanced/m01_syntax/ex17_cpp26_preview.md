# m01_syntax/ex17_cpp26_preview — Обзор C++26

> **Модуль:** `m01_syntax` · **Сложность:** ★★★☆☆ · **Время:** 40–60 мин
> **Запуск:** `./run.ps1 m01_syntax/ex17_cpp26_preview`

## Теория (кратко)

C++26 завершает десятилетие работы: **контракты**, **статическая рефлексия**,
**senders/receivers** для асинхронности, `std::inplace_vector`, `std::hive`,
hazard pointers, `#embed`, индексация пакетов `args...[N]`, деструктивное
удаление с обоснованием `= delete("reason")` и др.

На нашем тулчейне (GCC 16.2, `ucrt64`) доступна часть из этого. Этот пример —
**обзорная карта**: показывает, что уже работает, и помечает остальное как
«проверяется макросами». Не полагайтесь на неподдерживаемое в проде.

| Возможность | Статус на GCC 16.2 | Макрос |
|---|---|---|
| contracts (`pre`/`post`/`contract_assert`) | поддерживается | `__cpp_contracts` |
| pack indexing (`args...[0]`) | поддерживается | `__cpp_pack_indexing` |
| `std::inplace_vector` | поддерживается | `__cpp_lib_inplace_vector` |
| `= delete("reason")` | поддерживается | — |
| `std::hive` | нет | `__cpp_lib_hive` |
| senders/receivers (`std::execution`) | нет | `__cpp_lib_senders` |
| статическая рефлексия | экспериментально (`-freflection`) | `__cpp_reflection` |

## Синтаксис

```cpp
// contracts
int checked(int x)
    pre(x >= 0)          // preduslovie
    post(r: r >= 0)      // postuslovie (r - rezul'tat)
{ return x; }

// pack indexing
template <typename... Ts>
auto first(Ts... args) { return args...[0]; }

// inplace_vector: emkost' na steke, bez dinamiki
std::inplace_vector<int, 8> v;

// delete s prichinoj
NoCopy(const NoCopy&) = delete("NoCopy is not copyable");
```

## Подводные камни

- Контракты могут требовать флаг `-fcontracts` в зависимости от сборки; режим
  (enforce/observe/ignore) задаётся флагами `-fcontract-evaluation-semantics`.
- `std::inplace_vector::push_back` при переполнении **бросает**
  `std::bad_alloc`-подобную ошибку (`std::bad_alloc`/`std::length_error`) —
  заранее проверяйте `size() < capacity()`.
- Рефлексия пока экспериментальна: включайте только флагом, API меняется.
- Не закладывайтесь на C++26 в портируемом коде без проверки макросов
  `__cpp_*`.

## Задача

В `ex17_cpp26_preview.cpp`:

1. выведите отчёт о доступности C++26-макросов;
2. продемонстрируйте `std::inplace_vector` (вставка, `capacity`, отсутствие
   динамического роста);
3. покажите pack indexing (`args...[0]`);
4. при наличии `__cpp_contracts` — функцию с `pre`/`post`;
5. используйте `= delete("reason")`.

## Где применяется

- `inplace_vector` — плагины/ECS/аудио-буферы с жёстким лимитом и без аллокаций.
- Контракты — документирование и проверка инвариантов на границах API.
- Рефлексия (в будущем) — автоматическая сериализация, DI, биндинги.

## Проверка

Прогон печатает отчёт по макросам, размер/ёмкость `inplace_vector`, `first(...)`
и результат контракта; завершается `0`.

### Решение

См. `ex17_cpp26_preview.cpp`.
