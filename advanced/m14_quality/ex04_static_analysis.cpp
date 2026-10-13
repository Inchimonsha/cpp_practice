// m14_quality/ex04_static_analysis — staticheskiy analiz na urovne kompilyatora:
// static_assert, [[nodiscard]], consteval, if consteval, [[maybe_unused]].
// Sm. .md: real'naya proverka g++ -fanalyzer (podderzhivaetsya etim GCC).
// Standart: C++23. Zapusk: ./run.ps1 m14_quality/ex04_static_analysis
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <type_traits>

[[nodiscard]] constexpr int validated_multiply(int left, int right) {
    return left * right;
}

consteval std::array<int, 5> first_squares() {
    std::array<int, 5> result{};
    for (int index = 0; index < 5; ++index) {
        result[static_cast<std::size_t>(index)] = index * index;
    }
    return result;
}

constexpr bool table_valid() {
    const std::array<int, 5> table = first_squares();
    return table[0] == 0 && table[1] == 1 && table[2] == 4 && table[3] == 9 && table[4] == 16;
}

static_assert(table_valid(), "tablica kvadratov neverna");

template <class T>
[[nodiscard]] constexpr std::size_t type_size() {
    return sizeof(T);
}

int evaluation_phase() {
    if consteval {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    [[maybe_unused]] const int suppressed = 0;
    const int product = validated_multiply(6, 7);
    const std::array<int, 5> squares = first_squares();

    std::println("compile-time validation passed");
    std::println("validated_product = {}", product);
    std::println("squares[4] = {}", squares[4]);
    std::println("int size = {}", type_size<int>());
    std::println("evaluation phase = {} (runtime)", evaluation_phase());

    const bool ok = product == 42 && squares[4] == 16 && type_size<int>() == 4 &&
                    std::is_same_v<decltype(product), const int>;
    std::cout << "static analysis done\n";
    return ok ? 0 : 1;
}
