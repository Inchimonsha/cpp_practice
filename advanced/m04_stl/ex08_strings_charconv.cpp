// m04_stl/ex08_strings_charconv — stroki, charconv, format.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex08_strings_charconv
#include <charconv>
#include <format>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <system_error>

int main() {
    std::string_view input = "  42";
    while (!input.empty() && input.front() == ' ') {
        input.remove_prefix(1);
    }

    int value{};
    const auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
    std::println("parsed '{}' -> {} (ok={})",
                 std::string(input), value, ec == std::errc{} && ptr == input.data() + input.size());

    std::string_view bad = "12x";
    int junk{};
    const auto [bptr, bec] = std::from_chars(bad.data(), bad.data() + bad.size(), junk);
    std::println("parsed '{}' -> {} (all consumed={})",
                 std::string(bad), junk, bptr == bad.data() + bad.size());

    char buffer[64];
    const auto [iend, iec] = std::to_chars(buffer, buffer + sizeof(buffer), 2024);
    const std::string year{buffer, iend};
    const auto [dend, dec] = std::to_chars(buffer, buffer + sizeof(buffer), 3.14159);
    const std::string pi{buffer, dend};
    std::println("to_chars: year='{}' pi='{}'", year, pi);

    const std::string message = std::format("{} has {} points", "player", 1337);
    std::println("format: {}", message);

    std::string_view path = "assets/hero.png";
    const std::size_t slash = path.find('/');
    const std::string_view dir = path.substr(0, slash);
    std::string_view file = path;
    file.remove_prefix(slash + 1);
    const std::size_t dot = file.find('.');
    std::string_view stem = file.substr(0, dot);
    std::println("path: dir='{}' file='{}' stem='{}'", dir, file, stem);

    std::cout << "strings/charconv done\n";
    return (value == 42 && year == "2024" && stem == "hero") ? 0 : 1;
}
