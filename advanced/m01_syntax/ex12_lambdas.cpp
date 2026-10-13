// m01_syntax/ex12_lambdas — lyambdy.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex12_lambdas
#include <algorithm>
#include <iostream>
#include <print>
#include <vector>

namespace {
constexpr auto square_constexpr = [](int v) constexpr { return v * v; };
}  // namespace

int main() {
    static_assert(square_constexpr(5) == 25);

    std::vector<int> values{5, 1, 4, 2, 3};
    std::sort(values.begin(), values.end(),
              [](int a, int b) { return a > b; });  // po ubyvaniyu
    std::print("sorted desc:");
    for (const int v : values) { std::print(" {}", v); }
    std::print("\n");

    // Zaxvat po ssylke dlya schetchika.
    int even = 0;
    std::for_each(values.begin(), values.end(), [&even](int v) {
        if (v % 2 == 0) { ++even; }
    });
    std::println("even count = {}", even);

    // Sostoyanie zhivushchee vnutri lyambdy (kopiya + mutable).
    auto ticker = [count = 0]() mutable { return ++count; };
    const int t1 = ticker();
    const int t2 = ticker();
    const int t3 = ticker();
    std::println("ticker: {} {} {}", t1, t2, t3);

    // init-capture: vychislili i zaxvatili.
    const int limit = 4;
    auto is_small = [bound = limit * 2](int v) { return v < bound; };
    std::println("3 < bound? {}  9 < bound? {}", is_small(3), is_small(9));

    // Generic-lambda: odin kod dlya raznyh tipov.
    const auto cube = [](auto x) { return x * x * x; };
    std::println("cube(3) = {}, cube(1.5) = {}", cube(3), cube(1.5));

    std::cout << "lambdas done\n";
    return (values == std::vector<int>{5, 4, 3, 2, 1} && even == 2) ? 0 : 1;
}
