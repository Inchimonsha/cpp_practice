# ex03_benchmarking — микробенчмарки и `std::chrono`

## Теория

Микробенчмарк измеряет время небольшого фрагмента кода. Ключевые элементы
корректного замера:

- **Таймер**: `std::chrono::steady_clock` — монотонные часы, не «прыгают» при
  смене системного времени.
- **Много прогонов**: единичный замер шумный; берут несколько и считают
  минимум/медиану. Минимум устойчивее к вытеснению планировщиком.
- **Защита от оптимизации**: результат работы нужно «потребить», иначе
  компилятор удалит вычисление. Используют `volatile`-приёмник, `asm volatile`
  (GNU) или `DoNotOptimize`.
- **Разогрев и изоляция**: прогрейте кэши/частоты, не смешивайте измерения.

Важно измерять **правильную** вещь: узкое место может быть в аллокациях,
ветвлениях или памяти, а не в «строках арифметики».

## Синтаксис

```cpp
auto t0 = std::chrono::steady_clock::now();
work();
auto t1 = std::chrono::steady_clock::now();
double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

volatile std::uint64_t sink;      // chtoby rezul'tat ne optimizirovali
sink = work();
```

## Подводные камни

- Измерение одного вызова `now()` шумно; суммы/разности длительностей считайте в
  подходящих единицах (`duration<double, std::milli>`).
- Компилятор выбросит «мертвый» код — обязательно «потребляйте» результат.
- Частоты CPU меняются (turbo, энергосбережение); время зависит от машины и
  нагрузки.
- `clock()` измеряет процессорное время, `steady_clock` — настенные монотонные
  часы; не путайте.
- Не сравнивайте абсолютные числа между машинами — только относительные на одной.

## Задача

Замерить вычисление `sum_{i=1..n} i^2` для `n = 1'000'000` пять раз через
`steady_clock`, вывести минимум/медиану/максимум в миллисекундах. Результат
сверить с формулой `n(n+1)(2n+1)/6`. Результат вычисления «потребляется» через
`volatile`.

## Где применяется

- Оптимизация горячих путей, сравнение реализаций алгоритмов/структур,
  отслеживание регрессий производительности в CI.

## Проверка

Вывод: три времени в мс и сверка контрольной суммы; код возврата `0`, если
контрольная сумма совпала, иначе `1`. Абсолютные времена зависят от машины.

## Решение

```cpp
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <print>
#include <vector>

namespace {
volatile std::uint64_t g_sink = 0;

std::uint64_t sum_of_squares(int n) {
    std::uint64_t sum = 0;
    for (int i = 1; i <= n; ++i) {
        const auto v = static_cast<std::uint64_t>(i);
        sum += v * v;
    }
    g_sink = sum;   // "potreblenie" rezul'tata
    return sum;
}

std::uint64_t expected_sum(int n) {
    const std::uint64_t a = static_cast<std::uint64_t>(n);
    return a * (a + 1) * (2 * a + 1) / 6;
}
}  // namespace

int main() {
    constexpr int kRuns = 5;
    constexpr int kN = 1000000;

    std::vector<double> samples;
    samples.reserve(kRuns);
    std::uint64_t checksum = 0;
    for (int run = 0; run < kRuns; ++run) {
        const auto t0 = std::chrono::steady_clock::now();
        checksum = sum_of_squares(kN);
        const auto t1 = std::chrono::steady_clock::now();
        samples.push_back(
            std::chrono::duration<double, std::milli>(t1 - t0).count());
    }
    std::sort(samples.begin(), samples.end());

    std::println("timing (ms): min={: .4f} median={: .4f} max={: .4f}",
                 samples.front(), samples[samples.size() / 2], samples.back());
    std::println("checksum = {} expected = {}", checksum, expected_sum(kN));

    std::cout << "benchmarking done\n";
    return (checksum == expected_sum(kN)) ? 0 : 1;
}
```
