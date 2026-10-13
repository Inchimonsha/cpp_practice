# m03_templates/ex02_specialization_sfinae — Специализация и SFINAE

> **Модуль:** `m03_templates` · **Сложность:** ★★★★☆ · **Время:** 60–80 мин
> **Запуск:** `./run.ps1 m03_templates/ex02_specialization_sfinae`

## Теория (кратко)

**Специализация** даёт особую реализацию шаблона для конкретного типа
(полная) или образца типов (частичная). **SFINAE** (Substitution Failure Is Not
An Error) — правило: если подстановка аргумента в шаблон не удалась, этот
кандидат просто исключается из перегрузки, а не даёт ошибку. На этом строится
`std::enable_if` и **tag dispatch**.

| Приём | Идея |
|---|---|
| полная специализация | отдельный код для `bool`, `const char*` |
| частичная специализация | «общий случай» для `T*`, `vector<T>` |
| `std::enable_if_t` | включить перегрузку только при условии |
| tag dispatch | выбрать реализацию по `std::true_type`/`false_type` |

## Синтаксис

```cpp
template <class T> struct describe { static const char* text() { return "value"; } };
template <class T> struct describe<T*> { static const char* text() { return "pointer"; } };
template <>        struct describe<bool> { static const char* text() { return "boolean"; } };

template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
std::string only_integral(T) { return "integral"; }

template <class T>
std::string pick(T v) { return pick_impl(v, std::is_integral<T>{}); }  // tag dispatch
```

## Подводные камни

- Порядок объявлений: специализации должны быть видны до использования.
- `enable_if` в возвращаемом типе vs в параметре — распространённый стиль;
  в C++20 почти всегда заменяется concepts (`ex04`).
- SFINAE молча «выключает» перегрузки: ошибка проявляется как «no matching
  function», что трудно читать.
- Специализировать лучше на признаках (`is_integral`), чем на конкретных типах.

## Задача

В `ex02_specialization_sfinae.cpp`:

1. реализуйте `describe<T>` с частичной специализацией для `T*` и полной для
   `bool`;
2. ограничьте функцию `std::enable_if`'ом (только целочисленные);
3. реализуйте tag dispatch для выбора ветки по `is_integral`;
4. проверьте все ветки печатью;
5. добавьте частичную специализацию для `std::vector<T>` (дайте размер).

## Где применяется

- Разная сериализация/логирование для разных типов.
- Обобщённые алгоритмы, которым нужна «особая» ветка для указателей/чисел.
- Ограничение перегрузок до появления concepts (легаси-код, старые библиотеки).

## Проверка

Прогон печатает результаты для `int`, `bool`, указателя, `vector<int>` и
float-ветки; завершается `0`.

### Решение

См. `ex02_specialization_sfinae.cpp`.
