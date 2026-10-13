# m04_stl/ex04_iterators — Итераторы

> **Модуль:** `m04_stl` · **Сложность:** ★★★★☆ · **Время:** 60–80 мин
> **Запуск:** `./run.ps1 m04_stl/ex04_iterators`

## Теория (кратко)

Итератор — обобщённый «указатель» на элемент диапазона. Алгоритмы STL работают
через итераторы, а не через контейнеры. Категория итератора определяет, какие
операции и алгоритмы доступны.

| Категория | Умеет | Пример |
|---|---|---|
| input | читать по одному разу | `istream_iterator` |
| forward | идти вперёд, многопроходный | `forward_list` |
| bidirectional | назад | `list` |
| random_access | `+`/`-`, индексация | `vector`, `deque` |
| contiguous | непрерывная память | `vector`, `string` |

## Синтаксис

```cpp
auto it = data.begin();
std::advance(it, 3);            // sdvinut' na 3 (rabotaet dlya lюboj kategorii)
auto n = std::next(it);         // +1 (ne menyaet it)
auto p = std::prev(it, 2);

std::copy(src.begin(), src.end(), std::back_inserter(dst));

// iterator_traits
using Cat = std::iterator_traits<Iter>::iterator_category;
```

## Подводные камни

- Категория важна: `std::sort` требует random-access (не работает на `list`).
- `std::advance` для forward-итераторов на n шагов — O(n); для random-access — O(1).
- Инвалидация: изменение контейнера может «убить» итераторы.
- Свой итератор должен объявить трейты (`value_type`, `iterator_category`) и
  удовлетворять требованиям категории.

## Задача

В `ex04_iterators.cpp`:

1. покажите `advance`/`next`/`prev` и печать;
2. примените `back_inserter` с `std::copy`;
3. выведите категорию итератора `vector` через `iterator_traits`;
4. реализуйте свой forward-итератор для последовательности `start, start+step, …`;
5. сложите элементы своего диапазона через `std::accumulate`.

## Где применяется

- Обобщённые алгоритмы и range-based for.
- Адаптеры (`back_inserter`, `move_iterator`, `ostream_iterator`).
- Ленивые генераторы значений (свой итератор поверх источника данных).

## Проверка

Прогон печатает результаты операций с итераторами и сумму своего диапазона;
завершается `0`.

### Решение

См. `ex04_iterators.cpp`.
