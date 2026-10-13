// m03_templates/ex05_variadic_folds — variativnye shablony i fold-vyrazheniya.
// Standart: C++23. Zapusk: ./run.ps1 m03_templates/ex05_variadic_folds
#include <cstddef>
#include <iostream>
#include <print>
#include <utility>

namespace {

template <class... Ts>
[[nodiscard]] auto sum(Ts... args) {
    return (0 + ... + args);  // s nachal'nym znacheniem
}

template <class... Ts>
[[nodiscard]] std::size_t count_positive(Ts... args) {
    return (static_cast<std::size_t>(args > 0) + ... + 0);
}

template <class... Ts>
void print_all(Ts&&... args) {
    ((std::cout << args << ' '), ...);  // comma-fold, sleva napravo
    std::cout << '\n';
}

template <class T>
[[nodiscard]] constexpr const T& max2(const T& a, const T& b) {
    return a < b ? b : a;
}

template <class T, class... Ts>
[[nodiscard]] T max_of(T first, Ts... rest) {
    T best = first;
    ((best = max2(best, rest)), ...);
    return best;
}

template <class... Ts>
constexpr std::size_t count_of = sizeof...(Ts);

}  // namespace

int main() {
    std::println("sum(1,2,3,4)       = {}", sum(1, 2, 3, 4));
    std::println("sum()              = {}", sum());
    std::println("sum(1.5, 2.5)      = {}", sum(1.5, 2.5));

    std::println("count_positive(-1,0,2,3) = {}", count_positive(-1, 0, 2, 3));
    std::println("max_of(3,7,2,9,4)  = {}", max_of(3, 7, 2, 9, 4));

    std::print("print_all: ");
    print_all(1, 2.5, "three", 'c');

    std::println("sizeof...(int,double,char) = {}", count_of<int, double, char>);

    std::cout << "variadic folds done\n";
    return (sum(1, 2, 3) == 6 && max_of(3, 7, 2) == 7) ? 0 : 1;
}
