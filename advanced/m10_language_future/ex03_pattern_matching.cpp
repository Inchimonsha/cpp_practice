// m10_language_future/ex03_pattern_matching — razbor varianta cherez std::visit.
// C++26 inspect/is v GCC 16.2 net; pokazan rabotayushchiy fallback.
#include <cmath>
#include <format>
#include <iostream>
#include <numbers>
#include <optional>
#include <print>
#include <string>
#include <variant>

struct Circle { double r; };
struct Square { double side; };
struct Rect { double w; double h; };

using Shape = std::variant<Circle, Square, Rect>;

template <class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

double area(const Shape& shape) {
    return std::visit(overloaded{
        [](const Circle& c) { return std::numbers::pi * c.r * c.r; },
        [](const Square& q) { return q.side * q.side; },
        [](const Rect& r) { return r.w * r.h; },
    }, shape);
}

std::string describe(const Shape& shape) {
    return std::visit(overloaded{
        [](const Circle& c) { return "circle r=" + std::format("{}", c.r); },
        [](const Square& q) {
            return std::string(q.side >= 2.0 ? "large square" : "small square");
        },
        [](const Rect& r) { return "rect " + std::format("{}x{}", r.w, r.h); },
    }, shape);
}

std::optional<double> try_circle(const Shape& shape) {
    if (const auto* c = std::get_if<Circle>(&shape)) {
        return c->r;
    }
    return std::nullopt;
}

int main() {
#if defined(__cpp_pattern_matching)
    std::println("mode = inspect (C++26)");
#else
    std::println("mode = fallback (std::visit; GCC 16.2 bez inspect)");
#endif

    const Shape circle = Circle{2.0};
    const Shape square = Square{3.0};
    const Shape rect = Rect{2.0, 5.0};

    std::println("area(circle) = {:.4f}", area(circle));
    std::println("area(square) = {:.4f}", area(square));
    std::println("area(rect) = {:.4f}", area(rect));
    std::println("describe(circle) = {}", describe(circle));
    std::println("describe(square) = {}", describe(square));
    std::println("describe(rect) = {}", describe(rect));

    const auto radius = try_circle(circle);
    std::println("try_circle(circle) = {}", radius ? std::format("{}", *radius) : "nullopt");
    std::println("try_circle(rect) = {}",
                 try_circle(rect) ? std::format("{}", *try_circle(rect)) : "nullopt");

    const bool ok = std::abs(area(circle) - 12.566370614) < 1e-6 &&
                    area(square) == 9.0 && area(rect) == 10.0 &&
                    describe(square) == "large square" &&
                    radius.has_value() && *radius == 2.0 && !try_circle(rect).has_value();
    std::cout << "pattern matching done\n";
    return ok ? 0 : 1;
}
