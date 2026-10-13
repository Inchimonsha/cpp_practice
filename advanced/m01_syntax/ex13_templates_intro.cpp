// m01_syntax/ex13_templates_intro — vvedenie v shablony.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex13_templates_intro
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <type_traits>

namespace {

template <typename T>
[[nodiscard]] T maximum(T a, T b) {
    return a < b ? b : a;
}

// Klass-shablon s parametrom-znacheniem N.
template <typename T, std::size_t N>
struct Buffer {
    std::array<T, N> data{};

    [[nodiscard]] T sum() const {
        T total{};
        for (const T& v : data) { total += v; }
        return total;
    }
};

template <typename T>
[[nodiscard]] std::string kind() {
    if constexpr (std::is_integral_v<T>) {
        return "integral";
    } else if constexpr (std::is_floating_point_v<T>) {
        return "floating";
    } else {
        return "other";
    }
}

// Polnaya specializaciya dlya bool.
template <>
[[nodiscard]] std::string kind<bool>() {
    return "boolean";
}

}  // namespace

int main() {
    std::println("maximum(3, 7)     = {}", maximum(3, 7));
    std::println("maximum(2.5, 1.5) = {}", maximum(2.5, 1.5));

    Buffer<int, 4> buffer{};
    buffer.data = {1, 2, 3, 4};
    std::println("buffer.sum() = {}", buffer.sum());

    std::println("kind<int>   = {}", kind<int>());
    std::println("kind<double>= {}", kind<double>());
    std::println("kind<std::string> = {}", kind<std::string>());
    std::println("kind<bool>  = {}", kind<bool>());

    std::cout << "templates intro done\n";
    return buffer.sum() == 10 ? 0 : 1;
}
