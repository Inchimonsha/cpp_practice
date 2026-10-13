// m07_performance/ex03_benchmarking — mikrobendchmarki i std::chrono.
// Standart: C++23. Zapusk: ./run.ps1 m07_performance/ex03_benchmarking
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

    std::println("timing (ms): min={:.4f} median={:.4f} max={:.4f}",
                 samples.front(), samples[samples.size() / 2], samples.back());
    std::println("checksum = {} expected = {}", checksum, expected_sum(kN));

    std::cout << "benchmarking done\n";
    return (checksum == expected_sum(kN)) ? 0 : 1;
}
