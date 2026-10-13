# m12_parallel_gpu/ex03_simd_advanced — SIMD: маски, `where`, редукция

> Модуль m12_parallel_gpu · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m12_parallel_gpu/ex03_simd_advanced`

## Теория

SIMD-вектор обрабатывает несколько элементов одной инструкцией. Помимо
арифметики нужны **маски** — поэлементные логические результаты сравнений.
Маска позволяет:

- выбирать значения по условию (`where` / `select`);
- ограничивать диапазон (`clamp`);
- считать/сворачивать только выбранные линии (`reduce` над маской).

В `libstdc++` доступен заголовок `<experimental/simd>` (`namespace
std::experimental`, `stdx`). Есть `fixed_size_simd<T, N>` (ровно N линий) и
`simd<T>` (нативный размер). Стандартный `std::simd<T, N>` на этом тулчейне
пока отсутствует — используем экспериментальный.

Важная особенность реализации: **`stdx::select` не предоставляется**. Вместо
него — `where`-выражения: `where(mask, value) = ...`. Доступны `min`, `max`,
`clamp`, `minmax`, `abs`, `reduce`, `hmin`/`hmax`.

## Синтаксис

```cpp
namespace stdx = std::experimental;
using V = stdx::fixed_size_simd<float, 4>;
using M = V::mask_type;

V input(raw.data(), stdx::element_aligned);
V zero(0.0f), one(1.0f);

V clamped = stdx::clamp(input, zero, one);          // ogranichit' v [0,1]

M negative = input < zero;
V magnitude = input;
stdx::where(negative, magnitude) = stdx::abs(input); // vybor po maske

V indicator = zero;
stdx::where(stdx::abs(input) > V(2.0f), indicator) = one;
int count = static_cast<int>(stdx::reduce(indicator));  // skol'ko |x|>2

float dot = stdx::reduce(left * right);              // skalyarnoye proizvedeniye
```

## Подводные камни

- **Нет `select` в libstdc++** — используйте `stdx::where(mask, value) = ...`;
  это распространённая ловушка при копировании примеров из документации.
- **`element_aligned`**: загрузка из массива требует корректного выравнивания;
  для «произвольного» указателя — `stdx::unaligned`.
- **Печать элементов**: `v[i]` возвращает прокси-ссылку (`_SmartReference`), а не
  `float`; при выводе приводите явно `static_cast<float>(v[i])`.
- **`reduce` над маской**: реализация сворачивает значение, а не «считает
  истину» — сводите индикатор из нулей/единиц.
- Скаляр `V(2.0f)` **транслируется** во все линии (конструктор-бroadcast), это
  не размер вектора.
- Ширина `V::size()` фиксирована шаблоном (4), но `simd<T>` может быть шире —
  не полагайтесь на константу.

## Задача

Для вектора `{1.5, -2.0, 0.25, -3.5}`: ограничить в `[0,1]`; заменить
отрицательные на их модуль; посчитать элементы `|x| > 2`; вычислить скалярное
произведение `{1,2,3,4}·{5,6,7,8}`. Вывести по элементам.

## Где применяется

- Численные методы (физика, финансы), обработка сигналов и изображений,
  машинное обучение (аккумуляция, нормализация), аудио/видео кодеки.

## Проверка

Вывод: `simd width = 4`, `clamp01 = 1 0 0.25 0`, `sign-abs = 1.5 2 0.25 3.5`,
`count(|x| > 2) = 1`, `dot = 70`, `simd advanced done`. Код возврата `0` при
совпадении, иначе `1`.

## Решение

См. `ex03_simd_advanced.cpp`. Обратите внимание на `stdx::where(...) = ...` для
выбора по маске и `static_cast<float>` при печати элементов.
