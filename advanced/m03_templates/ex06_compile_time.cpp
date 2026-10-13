// m03_templates/ex06_compile_time — vychisleniya na etape kompilyacii.
// Standart: C++23. Zapusk: ./run.ps1 m03_templates/ex06_compile_time
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <type_traits>

namespace {

constexpr int fib(int n) {
    return n < 2 ? n : fib(n - 1) + fib(n - 2);
}

static_assert(fib(10) == 55);       // proverka na etape kompilyacii

consteval int square(int x) {       // tol'ko compile-time
    return x * x;
}

constexpr int sq = square(5);       // vyzov v constexpr-kontekste
static_assert(square(4) == 16);

constexpr std::array<int, 5> make_table() {
    std::array<int, 5> table{};
    for (std::size_t i = 0; i < table.size(); ++i) {
        table[i] = static_cast<int>(i * i);
    }
    return table;
}

constexpr auto kTable = make_table();   // tablica poschitana srazu

template <class T>
[[nodiscard]] std::string kind() {
    if constexpr (std::is_integral_v<T>) {
        return "integral";
    } else if constexpr (std::is_floating_point_v<T>) {
        return "floating";
    } else {
        return "other";
    }
}

constinit inline int kBase = fib(5);    // static-inicializaciya garantirovana

}  // namespace

int main() {
    std::println("fib(10) compile-time table start = {}", fib(10));
    std::println("square(5) as constexpr = {}", sq);

    std::print("table:");
    for (const int v : kTable) { std::print(" {}", v); }
    std::print("\n");

    std::println("kind<int>={} kind<double>={} kind<string>={}",
                 kind<int>(), kind<double>(), kind<std::string>());
    std::println("kBase = {}", kBase);

    std::cout << "compile time done\n";
    return (kTable[4] == 16 && sq == 25 && kBase == 5) ? 0 : 1;
}
