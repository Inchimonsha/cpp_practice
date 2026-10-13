// m04_stl/ex10_functional — funkcional'nye obyortki.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex10_functional
#include <functional>
#include <iostream>
#include <print>
#include <string>
#include <utility>
#include <vector>

namespace {

template <class F, class G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

void increment(int& value) { ++value; }

}  // namespace

int main() {
    std::println("std::function table:");
    const std::vector<std::pair<std::string, std::function<int(int)>>> ops{
        {"square", [](int x) { return x * x; }},
        {"negate", [](int x) { return -x; }},
        {"double", [](int x) { return x + x; }},
    };
    for (const auto& [name, op] : ops) {
        std::println("  {}(5) = {}", name, op(5));
    }

    std::println("compose:");
    auto add_then_square = compose([](int x) { return x * x; },
                                   [](int x) { return x + 1; });   // (x+1)^2
    std::println("  (3+1)^2 = {}", add_then_square(3));

    std::println("bind vs lambda:");
    auto plus10_bind = std::bind(std::plus<int>{}, std::placeholders::_1, 10);
    auto plus10_lambda = [](int x) { return x + 10; };
    std::println("  bind(7)   = {}", plus10_bind(7));
    std::println("  lambda(7) = {}", plus10_lambda(7));

    std::println("std::ref:");
    int counter = 5;
    increment(std::ref(counter));
    std::println("  after increment(counter) = {}", counter);

    std::println("functors directly:");
    std::println("  plus(2,3)   = {}", std::plus<int>{}(2, 3));
    std::println("  greater(5,3)= {}", std::greater<int>{}(5, 3));

    std::cout << "functional done\n";
    return (add_then_square(3) == 16 && counter == 6) ? 0 : 1;
}
