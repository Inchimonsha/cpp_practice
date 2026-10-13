// m01_syntax/ex16_cpp23_syntax — sintaksis C++23.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex16_cpp23_syntax
#include <bit>
#include <cstdint>
#include <expected>
#include <iostream>
#include <mdspan>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace {

[[nodiscard]] std::expected<int, std::string> parse_positive(int value) {
    if (value <= 0) {
        return std::unexpected("value must be positive");
    }
    return value;
}

// if consteval: raznye vetki dlya compile-time i runtime.
constexpr int trace(int x) {
    if consteval {
        return x * 2;  // na etape kompilyacii
    } else {
        return x * 2 + 1;  // v runtime
    }
}

}  // namespace

int main() {
    const auto good = parse_positive(7);
    const auto bad = parse_positive(-3);
    std::println("parse(7)  -> {}", good.value_or(-1));
    std::println("parse(-3) -> {}", bad.has_value() ? "ok" : bad.error());

    const std::vector<int> data{10, 20, 30};
    std::print("enumerate:");
    for (const auto& [index, value] : std::views::enumerate(data)) {
        std::print(" [{}]={}", index, value);
    }
    std::print("\n");

    auto doubled = data
                 | std::views::transform([](int v) { return v * 2; })
                 | std::ranges::to<std::vector>();
    std::print("doubled:");
    for (const int v : doubled) { std::print(" {}", v); }
    std::print("\n");

    const std::uint32_t original = 0x11223344u;
    const std::uint32_t swapped = std::byteswap(original);
    std::println("byteswap(0x11223344) = 0x{:08x}", swapped);

    constexpr int compile_time = trace(5);
    const int runtime_input = static_cast<int>(data.size()) + 2;  // ne svorachivaetsya
    const int runtime = trace(runtime_input);
    std::println("trace constexpr={} runtime={}", compile_time, runtime);

    // mdspan nad nepreryvnym buferom 2x3.
    std::vector<int> matrix{1, 2, 3, 4, 5, 6};
    std::mdspan<int, std::extents<std::size_t, 2, 3>> view{matrix.data()};
    std::println("matrix[1,2] = {}", view[1, 2]);

    std::cout << "cpp23 done\n";
    return (good.value() == 7 && swapped == 0x44332211u) ? 0 : 1;
}
