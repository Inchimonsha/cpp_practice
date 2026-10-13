// m01_syntax/ex06_scope_namespaces — oblasti vidimosti i namespace.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex06_scope_namespaces
#include <iostream>
#include <print>
#include <string>

namespace game::math {

constexpr double kGravity = 9.81;

[[nodiscard]] constexpr int clamp(int v, int lo, int hi) {
    if (v < lo) { return lo; }
    if (v > hi) { return hi; }
    return v;
}

}  // namespace game::math

// Vnutrennyaya svyaznost': vidno tol'ko v etom .cpp.
namespace {
int hidden_calls = 0;

int count_call() {
    ++hidden_calls;
    return hidden_calls;
}
}  // namespace

namespace geom {

struct Point {
    double x{};
    double y{};
};

// Svobodnaya funkciya v tom zhe namespace -> dostupna cherez ADL.
[[nodiscard]] std::string describe(const Point& p) {
    return "Point(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")";
}

}  // namespace geom

using game::math::clamp;  // tochechnoe podklyuchenie odnogo imeni

int main() {
    std::println("clamp(15, 0, 10) = {}", clamp(15, 0, 10));
    std::println("kGravity = {}", game::math::kGravity);

    const geom::Point p{1.5, 2.5};
    std::println("{}", describe(p));  // ADL najdet geom::describe

    const int first = count_call();
    const int second = count_call();
    std::println("count_call = {} {}", first, second);
    std::println("hidden_calls = {}", hidden_calls);

    std::cout << "scope/namespace done\n";
    return 0;
}
