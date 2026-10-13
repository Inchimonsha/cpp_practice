// m12_parallel_gpu/ex06_parallel_profiling — izmerenie uskoreniya (speedup).
// Vremya izmenyaetsya ot zapuska k zapusku (benchmark); proveryaem tol'ko rezul'tat.
// Standart: C++23. Zapusk: ./run.ps1 m12_parallel_gpu/ex06_parallel_profiling
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <print>
#include <thread>
#include <vector>

double serial_sum(std::size_t count) {
    double accumulator = 0.0;
    for (std::size_t index = 1; index <= count; ++index) {
        accumulator += std::sqrt(static_cast<double>(index));
    }
    return accumulator;
}

double parallel_sum(std::size_t count, unsigned workers) {
    std::vector<double> partial(workers, 0.0);
    {
        std::vector<std::jthread> pool;
        for (unsigned worker = 0; worker < workers; ++worker) {
            pool.emplace_back([&partial, count, workers, worker] {
                double accumulator = 0.0;
                for (std::size_t index = worker + 1; index <= count; index += workers) {
                    accumulator += std::sqrt(static_cast<double>(index));
                }
                partial[worker] = accumulator;
            });
        }
    }
    double total = 0.0;
    for (double value : partial) {
        total += value;
    }
    return total;
}

int main() {
    constexpr std::size_t count = 20'000'000;
    const unsigned workers = std::max(2u, std::thread::hardware_concurrency());

    const auto serial_start = std::chrono::steady_clock::now();
    const double serial = serial_sum(count);
    const auto serial_end = std::chrono::steady_clock::now();

    const auto parallel_start = std::chrono::steady_clock::now();
    const double parallel = parallel_sum(count, workers);
    const auto parallel_end = std::chrono::steady_clock::now();

    const double serial_ms =
        std::chrono::duration<double, std::milli>(serial_end - serial_start).count();
    const double parallel_ms =
        std::chrono::duration<double, std::milli>(parallel_end - parallel_start).count();
    const double speedup = serial_ms / parallel_ms;

    std::println("workers = {}, count = {}", workers, count);
    std::println("serial   = {:.3f}  elapsed = {:.3f} ms", serial, serial_ms);
    std::println("parallel = {:.3f}  elapsed = {:.3f} ms", parallel, parallel_ms);
    std::println("speedup = {:.2f}x", speedup);

    const bool ok = std::abs(serial - parallel) < 1e-6 * std::abs(serial);
    std::cout << "parallel profiling done\n";
    return ok ? 0 : 1;
}
