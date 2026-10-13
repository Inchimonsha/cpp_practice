// m00_tooling/ex02_compiler_flags — preduprezhdeniya i atributy.
// Standart: C++23. Zapusk: ./run.ps1 m00_tooling/ex02_compiler_flags
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <vector>

namespace util {

enum class Level { info, warning, error };

[[nodiscard]] std::string_view to_string(Level level) {
    switch (level) {
        case Level::info:    return "info";
        case Level::warning: return "warning";
        case Level::error:   return "error";
    }
    return "unknown";
}

// [[nodiscard]]: rezul'tat parsira nel'zya molcha poteryat'.
[[nodiscard]] bool parse_level(std::string_view text, Level& out,
                               [[maybe_unused]] bool strict = false) {
    if (text == "info")  { out = Level::info;    return true; }
    if (text == "warn")  { out = Level::warning; return true; }
    if (text == "error") { out = Level::error;   return true; }
    return false;
}

// Yavnye tipy i otsutstvie neyavnyh suzhenij: chisto dazhe pod -Wconversion.
[[nodiscard]] std::size_t total_bytes(const std::vector<std::string>& messages) {
    std::size_t sum = 0;
    for (const std::string& message : messages) {
        sum += message.size();
    }
    return sum;
}

}  // namespace util

namespace legacy {
[[deprecated("use util::parse_level")]] bool parse_level(const char* /*text*/) {
    return false;
}
}  // namespace legacy

int main() {
    util::Level level = util::Level::info;

    const std::string_view input = "warn";
    if (util::parse_level(input, level)) {
        std::println("parsed '{}' -> {}", input, util::to_string(level));
    }

    const std::vector<std::string> messages{"alpha", "beta", "gamma"};
    std::println("total bytes = {}", util::total_bytes(messages));

    // Suzhayushchee preobrazovanie delaem yavno (inache -Wconversion/-Werror).
    const double ratio = 0.75;
    const int percent = static_cast<int>(ratio * 100.0);
    std::println("percent = {}", percent);

    std::cout << "strict build: no warnings\n";
    return percent == 75 ? 0 : 1;
}
