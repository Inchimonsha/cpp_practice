// m14_quality/ex05_benchmarking_ci — mikrobendchmark i regressiohnyy gate.
// Vremya izmenyaetsya ot zapuska k zapusku (benchmark); proveryaem '=='.
// Standart: C++23. Zapusk: ./run.ps1 m14_quality/ex05_benchmarking_ci
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <vector>

std::string build_naive(std::size_t count) {
    std::string result;
    for (std::size_t index = 0; index < count; ++index) {
        result += std::to_string(index);
        result += ',';
    }
    return result;
}

std::string build_reserved(std::size_t count) {
    std::string result;
    result.reserve(count * 4);
    for (std::size_t index = 0; index < count; ++index) {
        result.append(std::to_string(index));
        result.push_back(',');
    }
    return result;
}

template <class Function>
double median_ms(Function function, int samples) {
    std::vector<double> times;
    times.reserve(static_cast<std::size_t>(samples));
    for (int sample = 0; sample < samples; ++sample) {
        const auto start = std::chrono::steady_clock::now();
        volatile std::size_t sink = function().size();
        static_cast<void>(sink);
        const auto end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

int main(int argc, char**) {
    const std::size_t count = 19999 + static_cast<std::size_t>(argc);
    constexpr int samples = 15;
    constexpr double budget_ms = 50.0;

    const std::string naive = build_naive(count);
    const std::string reserved = build_reserved(count);

    const double naive_ms = median_ms([count] { return build_naive(count); }, samples);
    const double reserved_ms = median_ms([count] { return build_reserved(count); }, samples);

    std::println("count = {}, samples = {}", count, samples);
    std::println("naive    = {} bytes, median = {:.3f} ms", naive.size(), naive_ms);
    std::println("reserved = {} bytes, median = {:.3f} ms", reserved.size(), reserved_ms);
    std::println("speedup = {:.2f}x", naive_ms / reserved_ms);

    const bool gate = naive_ms < budget_ms;
    std::println("regression gate (< {} ms): {}", budget_ms, gate ? "pass" : "fail");

    const bool ok = naive == reserved && gate;
    std::cout << "benchmarking ci done\n";
    return ok ? 0 : 1;
}
