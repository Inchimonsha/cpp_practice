# m04_stl/ex03_adaptors_heaps — Адаптеры и кучи

> **Модуль:** `m04_stl` · **Сложность:** ★★★☆☆ · **Время:** 40–60 мин
> **Запуск:** `./run.ps1 m04_stl/ex03_adaptors_heaps`

## Теория (кратко)

**Адаптеры** — обёртки над базовым контейнером, задающие дисциплину доступа:
`stack` (LIFO), `queue` (FIFO), `priority_queue` (максимум сверху). Основу
`priority_queue` составляет **куча** (binary heap) с операциями push/pop за
O(log n).

| Адаптер | Дисциплина | База по умолчанию |
|---|---|---|
| `stack` | LIFO | `deque` |
| `queue` | FIFO | `deque` |
| `priority_queue` | max-heap | `vector` |

Алгоритмы `<algorithm>`: `make_heap`, `push_heap`, `pop_heap`, `sort_heap`,
`is_heap`.

## Синтаксис

```cpp
std::stack<int> s;  s.push(1); s.top(); s.pop();
std::queue<int> q;  q.push(1); q.front(); q.pop();

std::priority_queue<int> maxHeap;                       // maximum sverhu
std::priority_queue<int, std::vector<int>, std::greater<>> minHeap;  // minimum

std::vector<int> data{3, 1, 4, 1, 5};
std::make_heap(data.begin(), data.end());               // teper' eto kucha
data.push_back(9);
std::push_heap(data.begin(), data.end());
std::pop_heap(data.begin(), data.end());                // max -> v konce
```

## Подводные камни

- `priority_queue` — не контейнер: нет итераторов, нет `begin/end`.
- Для min-heap нужен `std::greater<>` в качестве компаратора, а **не**
  отрицание значений.
- `push_heap`/`pop_heap` предполагают, что диапазон **уже** куча.
- `pop_heap` не удаляет элемент: он лишь переносит максимум в конец; удаляет
  `pop_back`.

## Задача

В `ex03_adaptors_heaps.cpp`:

1. `stack` — снимите элементы и напечатайте порядок LIFO;
2. `queue` — порядок FIFO;
3. `priority_queue` max и min, напечатайте порядок извлечения;
4. на `vector` примените `make_heap`/`push_heap`/`pop_heap` (кучная сортировка);
5. проверьте `is_heap`.

## Где применяется

- `stack` — обход в глубину, отмена операций, парсинг (сопоставление скобок).
- `queue` — BFS, буферы событий, планировщики FIFO.
- `priority_queue` — Dijkstra/A*, таймеры, очереди задач по приоритету.

## Проверка

Прогон печатает порядки извлечения и результат heapsort; завершается `0`.

### Решение

См. `ex03_adaptors_heaps.cpp`.
