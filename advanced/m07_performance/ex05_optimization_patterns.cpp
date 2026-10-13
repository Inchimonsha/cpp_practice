// m07_performance/ex05_optimization_patterns — patterny optimizacii.
// Standart: C++23. Zapusk: ./run.ps1 m07_performance/ex05_optimization_patterns
#include <iostream>
#include <print>
#include <vector>

namespace {
struct Tracker {
    static inline int copies = 0;
    int id;
    explicit Tracker(int value) : id(value) {}
    Tracker(const Tracker& other) : id(other.id) { ++copies; }
    Tracker(Tracker&& other) noexcept : id(other.id) {}
    Tracker& operator=(const Tracker&) = default;
    Tracker& operator=(Tracker&&) = default;
};

Tracker make_tracker(int id) {
    return Tracker{id};   // garantirovannaya eliziya (C++17)
}

int sum_likely(const std::vector<int>& values) {
    int sum = 0;
    for (int x : values) {
        if (x >= 0) [[likely]] {
            sum += x;
        } else {
            sum -= x;
        }
    }
    return sum;
}

int sum_expect(const std::vector<int>& values) {
    int sum = 0;
    for (int x : values) {
        if (__builtin_expect(x < 0, 0)) {
            sum -= x;
        } else {
            sum += x;
        }
    }
    return sum;
}
}  // namespace

int main() {
    Tracker tracker = make_tracker(7);
    std::println("RVO copies = {} (expected 0), id = {}",
                 Tracker::copies, tracker.id);

    const std::vector<int> values{1, -2, 3, -4, 5};
    const int a = sum_likely(values);
    const int b = sum_expect(values);
    std::println("sum_likely = {} sum_expect = {}", a, b);

    std::cout << "optimization patterns done\n";
    return (Tracker::copies == 0 && a == 15 && b == 15) ? 0 : 1;
}
