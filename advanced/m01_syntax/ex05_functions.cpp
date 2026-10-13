// m01_syntax/ex05_functions — funkcii.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex05_functions
#include <cmath>
#include <iostream>
#include <optional>
#include <print>

namespace {

constexpr double kPi = 3.14159265358979323846;

// Peregruzka po chislu/typu parametrov.
[[nodiscard]] constexpr double area(double radius) {
    return kPi * radius * radius;
}
[[nodiscard]] constexpr double area(double width, double height) {
    return width * height;
}

// Argumenty po umolchaniyu.
[[nodiscard]] int clamp(int value, int lo = 0, int hi = 100) {
    if (value < lo) { return lo; }
    if (value > hi) { return hi; }
    return value;
}

// Trailing return + constexpr.
[[nodiscard]] constexpr auto perimeter(double side) -> double {
    return 4.0 * side;
}

// noexcept: ne brosaet; oshibku peredaem cherez optional.
[[nodiscard]] std::optional<double> safe_div(double a, double b) noexcept {
    if (b == 0.0) { return std::nullopt; }
    return a / b;
}

}  // namespace

int main() {
    static_assert(perimeter(3.0) == 12.0);

    std::println("circle area r=2   = {}", area(2.0));
    std::println("rect area 3x4     = {}", area(3.0, 4.0));
    std::println("perimeter(5)      = {}", perimeter(5.0));

    std::println("clamp(150)        = {}", clamp(150));
    std::println("clamp(-3, -10, 10)= {}", clamp(-3, -10, 10));

    const std::optional<double> q = safe_div(10.0, 4.0);
    const std::optional<double> z = safe_div(1.0, 0.0);
    std::println("10/4 = {}  1/0 = {}", q.value_or(0.0), z.has_value());

    std::cout << "functions done\n";
    return (q.value() == 2.5 && !z.has_value()) ? 0 : 1;
}
