// m07_performance/ex02_cache_data_oriented — AoS vs SoA, lokal'nost' dannyh.
// Standart: C++23. Zapusk: ./run.ps1 m07_performance/ex02_cache_data_oriented
#include <array>
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
    explicit SoA(int n) : x(static_cast<std::size_t>(n)),
                          y(static_cast<std::size_t>(n)),
                          z(static_cast<std::size_t>(n)) {}
};
}  // namespace

int main() {
    AoS aos[kN];
    SoA soa(kN);
    for (int i = 0; i < kN; ++i) {
        aos[i] = {static_cast<float>(i),
                  static_cast<float>(2 * i),
                  static_cast<float>(3 * i)};
        soa.x[static_cast<std::size_t>(i)] = static_cast<float>(i);
        soa.y[static_cast<std::size_t>(i)] = static_cast<float>(2 * i);
        soa.z[static_cast<std::size_t>(i)] = static_cast<float>(3 * i);
    }

    float sum_aos = 0.0f;
    for (int i = 0; i < kN; ++i) { sum_aos += aos[i].x; }
    float sum_soa = 0.0f;
    for (int i = 0; i < kN; ++i) { sum_soa += soa.x[static_cast<std::size_t>(i)]; }

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
