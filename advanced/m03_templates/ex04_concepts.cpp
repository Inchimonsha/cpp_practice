// m03_templates/ex04_concepts — concepts i constraints.
// Standart: C++23. Zapusk: ./run.ps1 m03_templates/ex04_concepts
#include <concepts>
#include <iostream>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

namespace {

template <class T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <class T>
concept Sized = requires(T value) { value.size(); };

template <Numeric T>
[[nodiscard]] T add(T a, T b) {
    return a + b;
}

// Peregruzka: bolee specializirovannaya (Numeric) pobezhdaet.
std::string process(auto) {
    return "generic";
}

std::string process(Numeric auto) {
    return "numeric";
}

template <Sized T>
[[nodiscard]] auto size_of(const T& value) {
    return value.size();
}

template <Numeric T>
struct Accumulator {
    T total{};
    void add(T value) { total += value; }
};

}  // namespace

int main() {
    static_assert(Numeric<int>);
    static_assert(Numeric<double>);
    static_assert(!Numeric<std::string>);
    static_assert(Sized<std::vector<int>>);
    static_assert(!Sized<int>);

    std::println("add(2, 3)      = {}", add(2, 3));
    std::println("add(1.5, 2.5)  = {}", add(1.5, 2.5));

    std::println("process(42)          = {}", process(42));
    std::println("process(\"text\")      = {}", process(std::string("text")));

    std::println("size_of(vector) = {}", size_of(std::vector<int>{1, 2, 3, 4}));
    std::println("size_of(string) = {}", size_of(std::string("hello")));

    Accumulator<double> acc;
    acc.add(1.5);
    acc.add(2.25);
    std::println("accumulator.total = {}", acc.total);

    std::cout << "concepts done\n";
    return (process(1) == "numeric" && process(std::string{"x"}) == "generic") ? 0 : 1;
}
