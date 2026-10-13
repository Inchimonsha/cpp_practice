// m03_templates/ex01_template_basics — osnovy shablonov.
// Standart: C++23. Zapusk: ./run.ps1 m03_templates/ex01_template_basics
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <type_traits>

namespace {

// Funkciya-shablon: odin T (trebuet odinakovyh tipov).
template <class T>
[[nodiscard]] T maximum(T a, T b) {
    return a < b ? b : a;
}

// Dva parametra: raznye tipy + obshchij rezul'tat.
template <class T1, class T2>
[[nodiscard]] auto maximum(T1 a, T2 b) {
    using R = std::common_type_t<T1, T2>;
    return a < b ? static_cast<R>(b) : static_cast<R>(a);
}

// NTTP: razmer izvesten na etape kompilyacii.
template <std::size_t N>
struct Fixed {
    int data[N]{};

    [[nodiscard]] int sum() const {
        int total = 0;
        for (int v : data) { total += v; }
        return total;
    }
};

// Klass-shablon + CTAD.
template <class T>
struct Pair {
    T first;
    T second;
    Pair(T a, T b) : first(a), second(b) {}
};

// Deduction guide: dve stroki -> Pair<std::string>.
Pair(const char*, const char*) -> Pair<std::string>;

}  // namespace

int main() {
    std::println("maximum(3, 7)       = {}", maximum(3, 7));
    std::println("maximum(2, 2.5)     = {}", maximum(2, 2.5));
    std::println("maximum<double>(2, 2.5) = {}", maximum<double>(2, 2.5));

    Fixed<4> fixed;
    fixed.data[0] = 1;
    fixed.data[1] = 2;
    fixed.data[2] = 3;
    fixed.data[3] = 4;
    std::println("Fixed<4>.sum() = {}", fixed.sum());

    Pair p{1, 2};                       // CTAD -> Pair<int>
    static_assert(std::is_same_v<decltype(p), Pair<int>>);
    std::println("Pair p: {} {}", p.first, p.second);

    Pair names{"lazy", "fox"};          // guide -> Pair<std::string>
    static_assert(std::is_same_v<decltype(names), Pair<std::string>>);
    std::println("Pair names: '{}' '{}'", names.first, names.second);

    std::cout << "template basics done\n";
    return fixed.sum() == 10 && p.first == 1 ? 0 : 1;
}
