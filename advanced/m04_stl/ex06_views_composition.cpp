// m04_stl/ex06_views_composition — views i lenivye konvejery.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex06_views_composition
#include <iostream>
#include <print>
#include <ranges>
#include <vector>

int main() {
    const std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8};

    std::println("filter+transform+take -> vector:");
    auto pipeline = data
                  | std::views::filter([](int x) { return x % 2 == 0; })
                  | std::views::transform([](int x) { return x * x; })
                  | std::views::take(3);
    auto result = pipeline | std::ranges::to<std::vector>();
    std::print("  result:");
    for (int v : result) { std::print(" {}", v); }
    std::print("\n");

    std::println("laziness demo:");
    int calls = 0;
    auto lazy = data | std::views::transform([&calls](int x) {
        ++calls;
        return x * 10;
    });
    std::println("  calls before iteration = {}", calls);
    for (int v : lazy | std::views::take(2)) {
        std::println("  element {} -> calls = {}", v, calls);
    }

    std::println("enumerate:");
    std::print(" ");
    for (auto [index, value] : std::views::enumerate(data)) {
        std::print(" [{}]={}", index, value);
    }
    std::print("\n");

    std::println("iota + take:");
    std::print(" ");
    for (int v : std::views::iota(1, 100) | std::views::take(5)) {
        std::print(" {}", v);
    }
    std::print("\n");

    std::println("zip:");
    const std::vector<std::string> names{"a", "b", "c"};
    const std::vector<int> ids{10, 20};
    std::print(" ");
    for (auto [name, id] : std::views::zip(names, ids)) {
        std::print(" ({}, {})", name, id);
    }
    std::print("\n");

    std::cout << "views done\n";
    return (result.size() == 3 && result[0] == 4) ? 0 : 1;
}
