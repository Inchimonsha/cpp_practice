# m03_templates/ex01_template_basics — Основы шаблонов

> **Модуль:** `m03_templates` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m03_templates/ex01_template_basics`

## Теория (кратко)

Шаблон параметризуется типами и значениями. Компилятор **выводит** аргументы из
вызова (deduction), а с C++17 умеет выводить и параметры класса из конструктора
(**CTAD**). Для неоднозначных случаев пишут **deduction guides**.

| Средство | Пример |
|---|---|
| function template | `template <class T> T max(T, T)` |
| class template | `template <class T> struct Pair` |
| NTTP | `template <std::size_t N>` |
| CTAD | `Pair p{1, 2};` → `Pair<int>` |
| deduction guide | `Pair(const char*, const char*) -> Pair<std::string>` |

## Синтаксис

```cpp
template <class T>
[[nodiscard]] T maximum(T a, T b) { return a < b ? b : a; }

template <std::size_t N>
struct Fixed { int data[N]; };          // N — non-type template parameter

template <class T>
struct Pair {
    T first, second;
    Pair(T a, T b) : first(a), second(b) {}
};

Pair(const char*, const char*) -> Pair<std::string>;  // deduction guide
```

## Подводные камни

- Вывод из `maximum(1, 2.5)` **не сработает**: конфликт `T` (`int` и `double`).
  Нужны два параметра или явное приведение.
- CTAD не выводит из «ничего»: нужен конструктор с подходящими параметрами.
- NTTP должны быть известны на этапе компиляции.
- Объявление шаблона в `.hpp` и определение в `.cpp` разнесены → ошибка
  линковки (об этом `m08_build`).

## Задача

В `ex01_template_basics.cpp`:

1. напишите `maximum(T, T)` и `maximum(T1, T2)` (два параметра);
2. реализуйте `Fixed<N>` с NTTP и методом `sum()`;
3. сделайте `Pair<T>` и проверьте CTAD (`Pair{1, 2}` → `Pair<int>`);
4. добавьте deduction guide `Pair(const char*, const char*) -> Pair<std::string>`;
5. выведите выведенные типы через `if constexpr`/`typeid`-подобный приём.

## Где применяется

- Вся STL: контейнеры (`vector<T>`), утилиты (`pair`, `tuple`, `optional`).
- Учебные структуры фиксированного размера (буферы, матрицы) через NTTP.
- CTAD упрощает код: `std::pair p{1, 2.0}` вместо явных параметров.

## Проверка

Прогон печатает максимумы, сумму `Fixed<4>`, содержимое `Pair`; завершается `0`.

### Решение

См. `ex01_template_basics.cpp`.
