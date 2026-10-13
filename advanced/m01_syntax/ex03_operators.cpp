// m01_syntax/ex03_operators — operatory i peregruzka.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex03_operators
#include <cmath>
#include <compare>
#include <iostream>
#include <print>

namespace {

struct Vec2 {
    double x{};
    double y{};

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(double k) const { return {x * k, y * k}; }

    [[nodiscard]] double dot(const Vec2& o) const { return x * o.x + y * o.y; }
    [[nodiscard]] double length() const { return std::sqrt(dot(*this)); }

    bool operator==(const Vec2& o) const = default;
    auto operator<=>(const Vec2& o) const = default;  // po (x, potom y)
};

enum class Flags : unsigned {
    none   = 0u,
    hidden = 1u << 0,
    dirty  = 1u << 1,
    locked = 1u << 2,
};

Flags operator|(Flags a, Flags b) {
    return static_cast<Flags>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}
Flags operator&(Flags a, Flags b) {
    return static_cast<Flags>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
}
bool has(Flags set, Flags bit) { return (set & bit) == bit; }

int side_effect_counter = 0;
bool always_true() {
    ++side_effect_counter;
    return true;
}

}  // namespace

int main() {
    const Vec2 a{3.0, 4.0};
    const Vec2 b{1.0, 2.0};
    const Vec2 sum = a + b;
    std::println("a+b = ({}, {})", sum.x, sum.y);
    std::println("dot = {}, |a| = {}", a.dot(b), a.length());

    const Vec2 scaled = a * 2.0;
    std::println("a*2 = ({}, {})", scaled.x, scaled.y);

    Flags f = Flags::hidden | Flags::dirty;
    std::println("hidden? {}, locked? {}", has(f, Flags::hidden), has(f, Flags::locked));

    // Korotkoe zamykanie: pravyj operand ne vyzovetsya, schetchik ostanetsya 0.
    const bool short_circuit = false && always_true();
    std::println("false && ... = {}, side effects = {}", short_circuit, side_effect_counter);

    const bool checks = (sum == Vec2{4.0, 6.0})
                     && (a > b)
                     && has(f, Flags::dirty)
                     && (side_effect_counter == 0);
    std::println("all checks passed: {}", checks);

    std::cout << "operators done\n";
    return checks ? 0 : 1;
}
