# ex06_simd — векторизация и `std::simd`

## Теория

SIMD (single instruction, multiple data) — одна инструкция над несколькими
элементами. Два пути:

- **Автовекторизация**: компилятор сам превращает циклы в SIMD. Условия:
  тривиальные тела, отсутствие зависимостей между итерациями, выравнивание,
  доступный набор инструкций (`-mavx2`, `-march=native`). Диагностика:
  `-fopt-info-vec` (GCC).
- **Явный SIMD**: типы-векторы. В C++26 это `std::simd` из заголовка `<simd>`.
  В GCC 16.2 он **не предоставляется** (`__cpp_lib_simd` не определён), но есть
  расширение `<experimental/simd>` с `std::experimental::simd`.

`fixed_size_simd<T, N>` — вектор фиксированной ширины; `native_simd<T>` —
ширины, оптимальной под текущую цель (зависит от `-march`). `copy_from`,
поэлементные операции, `reduce` (свёртка) — типовой набор.

## Синтаксис

```cpp
namespace ex = std::experimental;   // GNU; v C++26 -> std::simd

using V4 = ex::fixed_size_simd<float, 4>;
V4 a(1.0f), b(2.0f);
V4 c = a + b;                     // pojelementno
float s = ex::reduce(a * b);      // skalyarnaya svertka
```

## Подводные камни

- `std::simd` — C++26; в GCC 16.2 нет (`__cpp_lib_simd` не определён). Держите код
  под feature-тестом и/или используйте `<experimental/simd>`.
- `native_simd` шире при `-march=native`; результат зависит от флагов и CPU.
- Невыровненные загрузки (`copy_from(..., element_unaligned)`) медленнее
  выровненных; проверяйте флаги выравнивания.
- Автовекторизация ломается из-за ветвлений, алиасинга, вызовов функций,
  зависимостей по данным.
- Ручной SIMD усложняет код и портирование; применяйте после измерений.

## Задача

Посчитать скалярное произведение векторов `a={1,2,3,4}`, `b={5,6,7,8}` (должно
быть `70`) двумя способами: обычным циклом и через `fixed_size_simd<float,4>` +
`reduce`. Вывести доступность `__cpp_lib_simd` и ширины `fixed_size_simd`/
`native_simd`. Запуск с `-march=native` расширит `native_simd`.

## Где применяется

- Математика и графика, обработка сигналов/изображений, ML-ядра, физика, сжатие,
  кодеки.

## Проверка

Вывод: `scalar dot = 70`, `simd dot = 70`; код возврата `0` при совпадении,
иначе `1`. Ширина `native_simd` зависит от `-march`.

## Решение

```cpp
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <version>

#include <experimental/simd>   // GNU; v C++26 ozhidaetsya <simd>

int main() {
#if defined(__cpp_lib_simd)
    std::println("std::simd: __cpp_lib_simd = {}",
                 static_cast<long>(__cpp_lib_simd));
#else
    std::println("std::simd: not available (GCC 16.2), using <experimental/simd>");
#endif

    namespace ex = std::experimental;
    using V4 = ex::fixed_size_simd<float, 4>;
    std::println("fixed_size_simd<float,4>::size() = {}", V4::size());
    std::println("native_simd<float>::size() = {}", ex::native_simd<float>::size());

    const std::array<float, 4> a{1.0f, 2.0f, 3.0f, 4.0f};
    const std::array<float, 4> b{5.0f, 6.0f, 7.0f, 8.0f};

    V4 va(0.0f);
    V4 vb(0.0f);
    for (std::size_t i = 0; i < 4; ++i) {
        va[i] = a[i];
        vb[i] = b[i];
    }

    const float dot = ex::reduce(va * vb);

    float scalar = 0.0f;
    for (std::size_t i = 0; i < 4; ++i) { scalar += a[i] * b[i]; }

    std::println("scalar dot = {}", scalar);
    std::println("simd   dot = {}", dot);

    std::cout << "simd done\n";
    return (scalar == dot && dot == 70.0f) ? 0 : 1;
}
```
