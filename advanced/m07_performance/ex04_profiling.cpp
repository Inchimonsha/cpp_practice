// m07_performance/ex04_profiling — profilirovanie goryachih putej.
// Standart: C++23. Zapusk: ./run.ps1 m07_performance/ex04_profiling
#include <chrono>
#include <cstdint>
#include <iostream>
#include <print>

namespace {
bool is_prime(int n) {
    if (n < 2) { return false; }
    if (n % 2 == 0) { return n == 2; }
    for (int d = 3; static_cast<std::int64_t>(d) * d <= n; d += 2) {
        if (n % d == 0) { return false; }
    }
    return true;
}

int count_primes(int limit) {
    int count = 0;
    for (int n = 2; n <= limit; ++n) {
        if (is_prime(n)) { ++count; }
    }
    return count;
}
}  // namespace

int main() {
    constexpr int kLimit = 100000;

    const auto t0 = std::chrono::steady_clock::now();
    const int count = count_primes(kLimit);
    const auto t1 = std::chrono::steady_clock::now();
    const double ms =
        std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::println("primes up to {} = {}", kLimit, count);
    std::println("elapsed = {:.2f} ms (machine-dependent)", ms);
    std::println("profile hint: g++ -O2 -g -pg ... && gprof; or: perf record -g");

    std::cout << "profiling done\n";
    return count == 9592 ? 0 : 1;
}
