// m01_syntax/ex15_cpp20_syntax — sintaksis C++20.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex15_cpp20_syntax
#include <array>
#include <concepts>
#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <vector>

namespace {

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
[[nodiscard]] T sum(std::span<const T> values) {
    T total{};
    for (const T& v : values) { total += v; }
    return total;
}

enum class Color { red, green, blue };

}  // namespace

constinit inline int g_tick = 42;  // garantirovannaya static-inicializaciya

int main() {
    const std::vector<int> data{1, 2, 3, 4, 5, 6};
    std::println("sum(vector) = {}", sum<int>(data));

    const double arr[]{1.5, 2.5, 3.0};
    std::println("sum(array)  = {}", sum<double>(arr));

    std::print("evens:");
    for (const int v : data | std::views::filter([](int x) { return x % 2 == 0; })) {
        std::print(" {}", v);
    }
    std::print("\n");

    const std::string message = std::format("{} items, sum = {}", data.size(), sum<int>(data));
    std::println("{}", message);

    using enum Color;  // teper' znacheniya dostupny bez Color::
    const Color chosen = green;
    std::println("chosen is green? {}", chosen == green);

    std::cout << "cpp20 done, g_tick=" << g_tick << '\n';
    return sum<int>(data) == 21 ? 0 : 1;
}
