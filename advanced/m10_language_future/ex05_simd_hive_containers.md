# m10_language_future/ex05_simd_hive_containers — SIMD и «бесконечно-пространственные» контейнеры

## Теория

Два независимых улучшения C++26, объединённые темой «работа с данными плотно и
эффективно»:

1. **`std::simd`** — переносимые векторные типы. Один объект держит `N`
   элементов и операции над ним превращаются в SIMD-инструкции (`SSE`, `AVX`,
   `NEON`). Главное: один код, разные платформы.
2. **Контейнеры без «узкого места» перераспределения/перемещения**:
   `std::hive` (сегментированный, стабильные ссылки) и `std::inplace_vector`
   (вектор с фиксированной ёмкостью на стеке, без динамической памяти).

На GCC 16.2: `std::simd` ещё не реализован как шаблон, `<hive>` отсутствует.
Рабочие эквиваленты — `std::experimental::simd` (`<experimental/simd>`) и
`std::inplace_vector` (`<inplace_vector>`, `__cpp_lib_inplace_vector=202603`).

## Синтаксис

```cpp
namespace stdx = std::experimental;
using V = stdx::native_simd<float>;             // shirina = shirina registra
constexpr std::size_t w = V::size();
V vx(ptr + i, stdx::element_aligned);           // zagruzka (load)
V r = a * vx + vy;                               // vektorizovannaya operaciya
r.copy_to(out + i, stdx::element_aligned);      // vygruzka (store)
float sum = stdx::reduce(v);                     // gorizontal'noe slozhenie

std::inplace_vector<int, 4> buf;                 // emkost' 4, bez heap
buf.push_back(3);                                // pri perepolnenii — bad_alloc
```

## Подводные камни

- `stdx::simd` **по умолчанию выравнивает** доступ; для сырых указателей нужно
  явно передавать тег выравнивания (`element_aligned` или `unaligned`).
- Инициализация списком `fixed_size_simd<float,4>{1,2,3,4}` не работает — грузите
  из массива/генератора.
- `V::size()` возвращает `int`; сравнивайте с `std::size_t` аккуратно.
- Хвост массива (длина не кратна ширине) нужно досчитывать скалярно — иначе
  потеря данных (как в `axpy`).
- `inplace_vector` **не растёт**: переполнение — `std::bad_alloc` (или
  `try_push_back`); ёмкость фиксирована параметром шаблона.
- `<hive>` в GCC 16.2 нет; `std::hive` даёт стабильные ссылки на элементы за счёт
  сегментов — это отдельная тема (задание-расширение).

## Задача

Реализовать `axpy` (`out = a*x + y`) с использованием `stdx::native_simd<float>` и
скалярным хвостом; продемонстрировать горизонтальное `reduce`; показать
`std::inplace_vector<int,4>` (несколько `push_back`, переполнение через
исключение). Проверить результаты.

## Где применяется

- Численные методы, обработка сигналов/изображений, физика, ML-инференс,
  пакетная обработка чисел; `inplace_vector` — для аллокационно-чувствительных
  участков и embedded.

## Проверка

Вывод: ширина SIMD (`4`), `axpy ok = true`, `reduce(1,2,3,4) = 10`,
`inplace_vector size=3 capacity=4`, `overflow rejected = true`. Код возврата `0`
при совпадении, иначе `1`. Запуск с `-Std c++26`.

## Решение

См. `ex05_simd_hive_containers.cpp`. Ключевые фрагменты:

```cpp
void axpy(float a, const float* x, const float* y, float* out, std::size_t n) {
    using V = stdx::native_simd<float>;
    constexpr std::size_t width = static_cast<std::size_t>(V::size());
    std::size_t i = 0;
    for (; i + width <= n; i += width) {
        const V vx(x + i, stdx::element_aligned);
        const V vy(y + i, stdx::element_aligned);
        (a * vx + vy).copy_to(out + i, stdx::element_aligned);
    }
    for (; i < n; ++i) {
        out[i] = a * x[i] + y[i];   // hvost
    }
}

std::inplace_vector<int, 4> buffer;
buffer.push_back(3);                // ... pri 5-m push_back: std::bad_alloc
```
