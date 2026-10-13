// m05_errors/ex04_error_strategies — strategii obrabotki oshibok.
// Standart: C++23. Zapusk: ./run.ps1 m05_errors/ex04_error_strategies
#include <expected>
#include <iostream>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>

namespace {

// 1) Isklyucheniya: dlya isklyuchitel'nyh situacij.
double divide_throws(double a, double b) {
    if (b == 0.0) { throw std::domain_error("division by zero"); }
    return a / b;
}

// 2) optional: dlya "net znacheniya" bez prichiny.
std::optional<double> divide_optional(double a, double b) {
    if (b == 0.0) { return std::nullopt; }
    return a / b;
}

// 3) expected: ozhidaemaya oshibka s prichinoj.
std::expected<double, std::string> divide_expected(double a, double b) {
    if (b == 0.0) { return std::unexpected("division by zero"); }
    return a / b;
}

}  // namespace

int main() {
    std::println("exceptions:");
    try {
        std::println("  10/4 = {}", divide_throws(10.0, 4.0));
        std::println("  10/0 = {}", divide_throws(10.0, 0.0));
    } catch (const std::domain_error& e) {
        std::println("  caught: {}", e.what());
    }

    std::println("optional:");
    if (auto r = divide_optional(10.0, 4.0)) {
        std::println("  10/4 = {}", *r);
    }
    if (!divide_optional(10.0, 0.0)) {
        std::println("  10/0 = no value");
    }

    std::println("expected:");
    if (auto r = divide_expected(10.0, 4.0)) {
        std::println("  10/4 = {}", *r);
    }
    if (auto r = divide_expected(10.0, 0.0); !r) {
        std::println("  10/0 = error: {}", r.error());
    }

    std::println("choice guidance:");
    std::println("  ozhidaemoe -> expected;  net znacheniya -> optional;");
    std::println("  isklyuchitel'noe -> exceptions;  invariant -> contracts/assert");

    std::cout << "error strategies done\n";
    return 0;
}
