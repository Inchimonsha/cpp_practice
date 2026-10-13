# m04_stl/ex02_associative_containers — Ассоциативные контейнеры

> **Модуль:** `m04_stl` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m04_stl/ex02_associative_containers`

## Теория (кратко)

Ассоциативные контейнеры хранят значения по ключу. **Упорядоченные**
(`map`/`set`) держат ключи отсортированными (красно-чёрное дерево, O(log n)),
**хеш-таблицы** (`unordered_map`/`unordered_set`) дают в среднем O(1), но без
порядка.

| Контейнер | Структура | Порядок | Средний поиск |
|---|---|---|---|
| `map` / `set` | дерево | по ключу | O(log n) |
| `unordered_map` / `unordered_set` | хеш-таблица | нет | O(1) |
| `multimap` / `multiset` | дерево | по ключу | дубликаты ключей |

## Синтаксис

```cpp
std::map<std::string, int> scores;
scores["alice"] = 10;
scores.insert_or_assign("bob", 20);
for (const auto& [name, score] : scores) { ... }   // otsortirovano po name

std::unordered_map<Point, int, PointHash> grid;    // svoj hasher

std::map<std::string, int, std::less<>> m;         // prozrachnyj komparator
m.find(std::string_view{"k"});                      // bez sozdaniya string
```

## Подводные камни

- `operator[]` у `map` **создаёт** значение по умолчанию, если ключа нет;
  для чтения используйте `find`/`at`.
- `unordered_map`: равные ключи → в одном бакете; порядок и даже число бакетов
  недетерминированы — не полагайтесь на порядок обхода.
- Плохой `hash` (например, всегда 0) вырождает таблицу в список → O(n).
- Для собственного типа нужны `operator==` и `std::hash` (или свой hasher).

## Задача

В `ex02_associative_containers.cpp`:

1. заполните `std::map<std::string,int>` и выведите в порядке ключей;
2. используйте `insert_or_assign` и проверьте `contains` (C++20);
3. примените `std::set<int>` для уникальности;
4. заведите `unordered_map` с кастомным типом ключа и своим hasher;
5. покажите прозрачный поиск (`std::less<>` + `string_view`).

## Где применяется

- `map` — конфиги, реестры ресурсов с детерминированным обходом.
- `unordered_map` — быстрые индексы/кэши (id → объект).
- `set` — дедупликация, проверка «входит ли».

## Проверка

Прогон печатает упорядоченные пары, результат поиска и попадания в хеш-таблицу;
завершается `0`.

### Решение

См. `ex02_associative_containers.cpp`.
