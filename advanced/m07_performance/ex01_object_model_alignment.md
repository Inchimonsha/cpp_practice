# ex01_object_model_alignment — модель объекта, выравнивание, `bit_cast`

## Теория

Раскладка объекта в памяти определяется размерами и **выравниванием**
(`alignof`) членов. Компилятор вставляет **padding**, чтобы каждый член был
выровнен; размер структуры кратен её выравниванию. Это влияет на
производительность (лишние байты) и на бинарную совместимость форматов.

- `alignof(T)` — требуемое выравнивание; `sizeof(T)` — размер с учётом padding.
- `offsetof(T, member)` — смещение члена в байтах (`<cstddef>`, для стандартно
  размещённых типов).
- `alignas(N)` — принудительное выравнивание.
- `std::bit_cast<To>(from)` (C++20) — безопасная переинтерпретация битов между
  типами одинакового размера. Заменяет `reinterpret_cast`/`memcpy` и корректно
  работает с оптимизатором.
- Strict aliasing: обращение к объекту через указатель несовместимого типа — UB;
  `bit_cast` даёт предсказуемый способ «увидеть» байты.

## Синтаксис

```cpp
struct A { char a; int b; char c; };      // padding posle a i c

static_assert(sizeof(A) == 12);

const auto bits = std::bit_cast<std::uint32_t>(1.0f);  // 0x3F800000
struct alignas(16) Vec4 { float x, y, z, w; };
```

## Подводные камни

- Размер и выравнивание зависят от ABI/платформы: не хардкодьте их без
  `static_assert` в целевом окружении.
- `offsetof` не определён для нестандартно размещённых типов (например, с
  `virtual`).
- `bit_cast` требует одинакового размера и тривиальной копируемости; для
  указателей на разные типы он не «приводит», а переинтерпретирует биты.
- Порядок байтов (endianness) для сериализации проверяйте
  `std::endian::native`.
- `reinterpret_cast` между несовместимыми типами нарушает strict aliasing.

## Задача

1. Вывести `sizeof`/`alignof` для `A{char;int;char}`, `B{int;char;char}` и
   `alignas(16) Vec4`, а также смещения полей `A`.
2. Через `bit_cast` получить биты `1.0f` (`0x3F800000`) и обратно.
3. Через `bit_cast` превратить `Vec4` в массив байтов и обратно.

## Где применяется

- Форматы сохранений и сетевые пакеты, interop с C/железом, выравнивание
  структур для SIMD/GPU, разбор бинарных данных.

## Проверка

Вывод содержит размеры, смещения, `0x3F800000` и восстановленный вектор; код
возврата `0` при корректности, иначе `1`.

## Решение

```cpp
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>

namespace {
struct A { char a; int b; char c; };
struct B { int b; char a; char c; };
struct alignas(16) Vec4 { float x, y, z, w; };
}  // namespace

int main() {
    std::println("layout:");
    std::println("  A:    sizeof={} alignof={}", sizeof(A), alignof(A));
    std::println("  B:    sizeof={} alignof={}", sizeof(B), alignof(B));
    std::println("  Vec4: sizeof={} alignof={}", sizeof(Vec4), alignof(Vec4));

    std::println("offsets in A:");
    std::println("  a={} b={} c={}", offsetof(A, a), offsetof(A, b), offsetof(A, c));

    std::println("bit_cast:");
    const float one = 1.0f;
    const auto bits = std::bit_cast<std::uint32_t>(one);
    std::println("  1.0f -> 0x{:08X}", bits);
    const float back = std::bit_cast<float>(bits);
    std::println("  back = {}", back);

    std::println("bytes <-> struct:");
    const Vec4 v{1.0f, 2.0f, 3.0f, 4.0f};
    const auto raw = std::bit_cast<std::array<std::byte, sizeof(Vec4)>>(v);
    const Vec4 restored = std::bit_cast<Vec4>(raw);
    std::println("  restored = ({}, {}, {}, {})",
                 restored.x, restored.y, restored.z, restored.w);

    std::cout << "object model done\n";
    return (bits == 0x3F800000u && back == 1.0f && restored.w == 4.0f) ? 0 : 1;
}
```
