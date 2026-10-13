// m12_parallel_gpu/ex05_false_sharing — lozhnoye razdelenie (false sharing).
// Vremya izmenyaetsya ot zapuska k zapusku (benchmark); proveryaem tol'ko summy.
// Standart: C++23. Zapusk: ./run.ps1 m12_parallel_gpu/ex05_false_sharing
#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <print>
#include <thread>
#include <vector>

struct alignas(64) PaddedCounter {
    std::atomic<long long> value{0};
};

int main() {
    constexpr unsigned threads = 4;
    constexpr int iterations = 2'000'000;

    std::vector<std::atomic<long long>> packed(threads);
    for (auto& counter : packed) {
        counter.store(0, std::memory_order_relaxed);
    }
    const auto packed_start = std::chrono::steady_clock::now();
    {
        std::vector<std::jthread> pool;
        for (unsigned index = 0; index < threads; ++index) {
            pool.emplace_back([&packed, index] {
                for (int step = 0; step < iterations; ++step) {
                    packed[index].fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
    }
    const auto packed_end = std::chrono::steady_clock::now();

    std::vector<PaddedCounter> padded(threads);
    const auto padded_start = std::chrono::steady_clock::now();
    {
        std::vector<std::jthread> pool;
        for (unsigned index = 0; index < threads; ++index) {
            pool.emplace_back([&padded, index] {
                for (int step = 0; step < iterations; ++step) {
                    padded[index].value.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
    }
    const auto padded_end = std::chrono::steady_clock::now();

    long long packed_total = 0;
    for (auto& counter : packed) {
        packed_total += counter.load(std::memory_order_relaxed);
    }
    long long padded_total = 0;
    for (auto& counter : padded) {
        padded_total += counter.value.load(std::memory_order_relaxed);
    }

    const double packed_ms =
        std::chrono::duration<double, std::milli>(packed_end - packed_start).count();
    const double padded_ms =
        std::chrono::duration<double, std::milli>(padded_end - padded_start).count();

    std::println("threads = {}, iterations = {}", threads, iterations);
    std::println("packed total = {}  elapsed = {:.3f} ms", packed_total, packed_ms);
    std::println("padded total = {}  elapsed = {:.3f} ms", padded_total, padded_ms);

    const long long expected = static_cast<long long>(threads) * iterations;
    const bool ok = packed_total == expected && padded_total == expected;
    std::cout << "false sharing done\n";
    return ok ? 0 : 1;
}
