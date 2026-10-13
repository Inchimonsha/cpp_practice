// m04_stl/ex11_sum_product_types — summy i proizvedeniya tipov.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex11_sum_product_types
#include <any>
#include <iostream>
#include <optional>
#include <print>
#include <string>
#include <tuple>
#include <variant>

namespace {

std::optional<int> parse_positive(int value) {
    if (value <= 0) { return std::nullopt; }
    return value;
}

}  // namespace

int main() {
    std::println("optional:");
    std::println("  parse_positive(5)  = {}", parse_positive(5).value_or(-1));
    std::println("  parse_positive(-3) = {}", parse_positive(-3).value_or(-1));

    std::println("variant + visit:");
    const std::variant<int, std::string, double> a = 42;
    const std::variant<int, std::string, double> b = std::string("hello");
    const std::variant<int, std::string, double> c = 2.5;
    const auto show = [](const auto& value) { std::println("    value = {}", value); };
    std::visit(show, a);
    std::visit(show, b);
    std::visit(show, c);

    std::println("variant get_if:");
    if (const auto* p = std::get_if<std::string>(&b)) {
        std::println("  b holds string '{}'", *p);
    }
    std::println("  a holds double? {}", std::holds_alternative<double>(a));

    std::println("any:");
    std::any box = std::string("boxed");
    if (auto* p = std::any_cast<std::string>(&box)) {
        std::println("  any_cast<string> = '{}'", *p);
    }
    if (std::any_cast<int>(&box) == nullptr) {
        std::println("  any_cast<int> = null (wrong type)");
    }

    std::println("tuple:");
    const auto record = std::tuple{1, std::string("hero"), 3.5};
    const auto& [id, name, health] = record;
    std::println("  id={} name={} health={}", id, name, health);
    std::println("  get<0>={}", std::get<0>(record));

    std::apply([](int i, const std::string& n, double h) {
        std::println("  apply: {} {} {}", i, n, h);
    }, record);

    std::cout << "sum/product types done\n";
    return (parse_positive(5) == 5 && std::holds_alternative<int>(a)) ? 0 : 1;
}
