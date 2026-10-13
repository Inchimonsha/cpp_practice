# m03_templates/ex03_type_traits — Type traits и detection idiom

> **Модуль:** `m03_templates` · **Сложность:** ★★★★☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m03_templates/ex03_type_traits`

## Теория (кратко)

`<type_traits>` — библиотека запросов о типах (значения `::value`/`_v`) и их
преобразований (`::type`/`_t`). На их основе пишут **пользовательские traits** и
**detection idiom** — проверку «есть ли у типа такой-то метод/член» без ошибки
компиляции.

| Категория | Примеры |
|---|---|
| запросы | `is_integral_v`, `is_pointer_v`, `is_same_v` |
| преобразования | `remove_reference_t`, `decay_t`, `common_type_t` |
| условия | `enable_if_t`, `conditional_t` |
| detection | `void_t` для «обнаружения» члена |

## Синтаксис

```cpp
template <class, class = void>
struct has_size : std::false_type {};

template <class T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

static_assert(has_size<std::vector<int>>::value);
static_assert(!has_size<int>::value);

using Plain = std::remove_reference_t<int&>;   // int
```

## Подводные камни

- Забыли `std::declval` (используйте `declval<T>()`, а не `T{}`): проверка
  требует «выражения», не объекта.
- `void_t`-детекция находит метод даже через неявные преобразования — иногда
  это не то, что нужно.
- `decay_t` убирает ссылки/`const`/массивы → «значение как по значению».
- В C++20 для «требует метод» удобнее concepts (`requires { t.size(); }`).

## Задача

В `ex03_type_traits.cpp`:

1. напишите `has_size<T>` через `void_t`;
2. напишите `has_value_type<T>` (вложенный `value_type`);
3. покажите `remove_reference_t`/`decay_t`/`common_type_t`;
4. выведите набор `is_*` для `int`, `double`, `std::vector<int>`, `int*`;
5. примените `if constexpr` для условной логики по traits.

## Где применяется

- Обобщённые алгоритмы: «если есть `.size()` — берём размер».
- Валидация интерфейсов на этапе компиляции (детекция `begin`/`end`, `serialize`).
- Ограничение перегрузок и выбор стратегии по свойствам типа.

## Проверка

Прогон печатает результаты trait-запросов; завершается `0`.

### Решение

См. `ex03_type_traits.cpp`.
