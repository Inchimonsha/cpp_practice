// m01_syntax/ex14_exceptions_basics — isklyucheniya.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex14_exceptions_basics
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

struct DivideByZero : std::runtime_error {
    DivideByZero() : std::runtime_error("division by zero") {}
};

[[nodiscard]] double divide(double a, double b) {
    if (b == 0.0) {
        throw DivideByZero{};
    }
    return a / b;
}

// RAII-markер: pechataet vhod/vyhod. Pri raskrutke ~ScopeScope vyzovetsya.
struct Tracer {
    std::string name;
    explicit Tracer(std::string n) : name(std::move(n)) {
        std::println("  enter {}", name);
    }
    ~Tracer() { std::println("  leave {}", name); }
};

[[nodiscard]] double risky(double a, double b) {
    const Tracer guard{"risky"};
    return divide(a, b);  // isklyuchenie raskrutit stek cherez guard
}

[[nodiscard]] int safe_parse_len(const std::string& text) noexcept {
    return static_cast<int>(text.size());
}

}  // namespace

int main() {
    try {
        std::println("10/4 = {}", divide(10.0, 4.0));
        std::println("about to divide by zero...");
        const double bad = risky(1.0, 0.0);
        std::println("never printed: {}", bad);
    } catch (const DivideByZero& e) {
        std::println("caught DivideByZero: {}", e.what());
    } catch (const std::exception& e) {
        std::println("caught std::exception: {}", e.what());
    }

    std::println("safe_parse_len(\"hello\") = {}", safe_parse_len("hello"));
    std::cout << "exceptions done\n";
    return 0;
}
