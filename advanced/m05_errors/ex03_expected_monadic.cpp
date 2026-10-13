// m05_errors/ex03_expected_monadic — std::expected i monadicheskie operacii.
// Standart: C++23. Zapusk: ./run.ps1 m05_errors/ex03_expected_monadic
#include <charconv>
#include <expected>
#include <iostream>
#include <print>
#include <string>
#include <string_view>

namespace {

using ParseResult = std::expected<int, std::string>;

ParseResult parse_int(std::string_view text) {
    int value{};
    const auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec != std::errc{} || ptr != text.data() + text.size()) {
        return std::unexpected(std::string{"not an integer"});
    }
    return value;
}

ParseResult validate_positive(int value) {
    if (value <= 0) {
        return std::unexpected(std::string{"must be positive"});
    }
    return value;
}

std::string run(std::string_view text) {
    auto result = parse_int(text)
        .and_then(validate_positive)
        .transform([](int v) { return v * 2; });

    if (result) {
        return "ok=" + std::to_string(*result);
    }
    return "error: " + result.error();
}

}  // namespace

int main() {
    std::println("parse+validate+double:");
    std::println("  \"21\"  -> {}", run("21"));
    std::println("  \"0\"   -> {}", run("0"));
    std::println("  \"-5\"  -> {}", run("-5"));
    std::println("  \"abc\" -> {}", run("abc"));

    std::println("or_else / value_or:");
    const auto good = parse_int("7").and_then(validate_positive);
    std::println("  value_or = {}", good.value_or(-1));

    const auto bad = parse_int("0")
        .and_then(validate_positive)
        .or_else([](const std::string& error) -> ParseResult {
            std::println("  or_else handled: {}", error);
            return -100;   // zapasnoe znachenie
        });
    std::println("  recovered = {}", *bad);

    std::cout << "expected monadic done\n";
    return (good.value_or(-1) == 7 && *bad == -100) ? 0 : 1;
}
