# m01_syntax/ex13_templates_intro — Введение в шаблоны

> **Модуль:** `m01_syntax` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m01_syntax/ex13_templates_intro`

## Теория (кратко)

Шаблон — «код с параметром-типом (или значением)», из которого компилятор
генерирует конкретные версии. Это основа контейнеров и алгоритмов STL. Вывод
типов из аргументов (deduction), частичная/полная специализация и `if constexpr`
позволяют писать обобщённый код без дублирования.

| Средство | Назначение |
|---|---|
| function template | одна функция для многих типов |
| class template | обобщённый контейнер/обёртка |
| deduction | вывод `T` из аргументов |
| specialization | особая реализация для конкретного типа |
| NTTP | параметр-значение (`<int N>`) |
| `if constexpr` | отбор кода на этапе компиляции |

## Синтаксис

```cpp
template <typename T>
[[nodiscard]] T maximum(T a, T b) { return a < b ? b : a; }

template <typename T, std::size_t N>
struct Buffer { std::array<T, N> data{}; };   // NTTP N

template <typename T>
std::string kind() {
    if constexpr (std::is_integral_v<T>) { return "integral"; }
    else { return "other"; }
}

template <>
std::string kind<bool>() { return "boolean"; }  // polnaya specializaciya
```

## Подводные камни

- Шаблон компилируется целиком при инстанцировании: ошибки видны только для
  «плохого» типа и выглядят громоздко (лечится concepts — `m03`).
- Определение шаблона обычно должно быть в заголовке (иначе отдельные `.cpp` не
  найдут его при инстанцировании).
- Специализация должна быть объявлена до первого использования.

## Задача

В `ex13_templates_intro.cpp`:

1. напишите `template <typename T> T maximum(T, T)`;
2. сделайте класс `template <typename T, std::size_t N> struct Buffer` с `sum()`;
3. реализуйте `kind<T>()` через `if constexpr` (integral/fp/other);
4. добавьте полную специализацию `kind<bool>()`;
5. проверьте вывод типов и результат сложения.

## Где применяется

- Все контейнеры STL — шаблоны (`vector<T>`, `map<K,V>`).
- Обёртки с фиксированным размером (`std::array<T,N>`), буферы на стеке.
- Политика поведения, выбираемая `if constexpr` без рантайм-ветвлений.

## Проверка

Прогон печатает максимумы, сумму буфера и «вид» типов; завершается `0`.

### Решение

См. `ex13_templates_intro.cpp`.
