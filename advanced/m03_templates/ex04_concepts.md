# m03_templates/ex04_concepts — Concepts и constraints

> **Модуль:** `m03_templates` · **Сложность:** ★★★★☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m03_templates/ex04_concepts`

## Теория (кратко)

**Concept** — именованное ограничение на параметры шаблона. Оно заменяет
громоздкий SFINAE, даёт понятные сообщения об ошибках и позволяет
**упорядочивать** перегрузки через **subsumption** (более строгое ограничение
«поглощает» более слабое).

| Инструмент | Смысл |
|---|---|
| `concept C = ...;` | объявление concept |
| `requires { ... }` | требуется, чтобы выражения были валидны |
| `requires(T a) { a.x; }` | requires-выражение с параметрами |
| `template <C T>` | применение к параметру |
| `f(C auto x)` | сокращённая запись |

## Синтаксис

```cpp
template <class T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <class T>
concept Sized = requires(T t) { t.size(); };          // est' metod size()

template <Numeric T>
T add(T a, T b) { return a + b; }                     // ogranichenie

void f(auto x) {};              // bez ogranicheniya
void f(Numeric auto x) {};      // bolee specializirovan -> predpochten
```

## Подводные камни

- Ограничение должно быть «логичным»: слишком слабый concept не поймает ошибку
  раньше, чем тело.
- `requires` в двух формах: `requires constraint` (clause) и
  `requires(...) { ... }` (expression) — не путайте.
- Subsumption работает только для concepts, связанных через `&&`/выводимость;
  произвольные `enable_if` не упорядочиваются.
- Не дублируйте проверки: concept + `if constexpr` может быть избыточно.

## Задача

В `ex04_concepts.cpp`:

1. объявите `concept Numeric` и `concept Sized`;
2. сделайте `add<Numeric T>`; проверьте, что нечисловой тип запрещён;
3. задайте `process(auto)` и `process(Numeric auto)` — покажите выбор;
4. используйте `requires`-выражение `Sized` для функции, берущей размер;
5. добавьте шаблон класса, ограниченный `Numeric`.

## Где применяется

- Публичные шаблонные API: внятные ошибки вместо «стены» SFINAE.
- Проверка требований к типам (например, «сериализуемый», «имеет `begin`/`end`»).
- Упорядочивание перегрузок без `enable_if`.

## Проверка

Прогон печатает результаты и выбор перегрузок; завершается `0`.

### Решение

См. `ex04_concepts.cpp`.
