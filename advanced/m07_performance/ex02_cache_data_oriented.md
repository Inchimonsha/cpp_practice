# ex02_cache_data_oriented — AoS vs SoA, локальность данных

## Теория

Производительность часто упирается не в число инструкций, а в **промахи кэша**.
Данные, которые обрабатываются вместе, выгодно располагать рядом (принцип
локальности). Отсюда два представления:

- **AoS** (array of structs): `struct Particle { x, y, z; }; Particle p[N];` —
  удобно, когда нужны все поля одной сущности.
- **SoA** (struct of arrays): `struct Particles { vector<float> x, y, z; };` —
  когда одна операция проходит по одному полю всех сущностей: последовательный
  доступ, меньше трат на лишние байты кэшлайна.

Кэшлайны на x86 — обычно **64 байта**. AoS-сумма по полю `x` загружает в кэш
лишние `y`,`z`; SoA — только `x`.

## Синтаксис

```cpp
struct AoS { float x, y, z; };
AoS a[N];

struct SoA { std::vector<float> x, y, z; };

for (int i = 0; i < N; ++i) sum += a[i].x;   // AoS: strided
for (int i = 0; i < N; ++i) sum += soa.x[i]; // SoA: linejno
```

## Подводные камни

- SoA усложняет код: нужны явные батч-операции, неудобно, когда сущность берут
  целиком.
- Выигрыш есть только при обработке поля пачкой; для «одного элемента со всеми
  полями» лучше AoS.
- Маленькие N могут целиком помещаться в кэш — разницы не будет.
- Не доверяйте «очевидному» превосходству: измеряйте (см. ex03, ex04).
- Выравнивание и `padding` тоже влияют (см. ex01).

## Задача

Для `N = 64` частиц заполнить AoS и SoA одинаковыми значениями, посчитать сумму
поля `x` в обоих представлениях (итог одинаков и равен `0+1+…+63 = 2016`) и
сравнить «затронутые байты»: в AoS грубо `N*sizeof(AoS)`, в SoA — `N*sizeof(float)`.

## Где применяется

- ECS, физика и системы частиц, батч-обработка, ETL, обработка потоков кадров,
  численные методы.

## Проверка

Вывод: обе суммы равны `2016`, и SoA «касается» меньше байтов; код возврата `0`
при совпадении сумм, иначе `1`.

## Решение

```cpp
#include <cstddef>
#include <iostream>
#include <print>
#include <vector>

namespace {
constexpr int kN = 64;
constexpr int kCacheLine = 64;

struct AoS { float x, y, z; };

struct SoA {
    std::vector<float> x, y, z;
    explicit SoA(int n) : x(n), y(n), z(n) {}
};
}  // namespace

int main() {
    AoS aos[kN];
    SoA soa(kN);
    for (int i = 0; i < kN; ++i) {
        const float f = static_cast<float>(i);
        aos[i] = {f, 2.0f * f, 3.0f * f};
        soa.x[static_cast<std::size_t>(i)] = f;
        soa.y[static_cast<std::size_t>(i)] = 2.0f * f;
        soa.z[static_cast<std::size_t>(i)] = 3.0f * f;
    }

    float sum_aos = 0.0f;
    float sum_soa = 0.0f;
    for (int i = 0; i < kN; ++i) { sum_aos += aos[i].x; }
    for (int i = 0; i < kN; ++i) { sum_soa += soa.x[i]; }

    std::println("layout:");
    std::println("  sizeof(AoS) = {}  cache line = {}", sizeof(AoS), kCacheLine);
    std::println("sum of x:");
    std::println("  AoS = {} (bytes ~{})",
                 sum_aos, kN * static_cast<int>(sizeof(AoS)));
    std::println("  SoA = {} (bytes ~{})",
                 sum_soa, kN * static_cast<int>(sizeof(float)));

    std::cout << "cache oriented done\n";
    return (sum_aos == sum_soa && sum_soa == 2016.0f) ? 0 : 1;
}
```
