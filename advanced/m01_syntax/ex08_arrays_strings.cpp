// m01_syntax/ex08_arrays_strings — massivy, stroki, span.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex08_arrays_strings
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <span>
#include <string>
#include <string_view>

namespace {

[[nodiscard]] int total(std::span<const int> values) {
    int sum = 0;
    for (const int v : values) {
        sum += v;
    }
    return sum;
}

[[nodiscard]] bool is_identifier(std::string_view text) {
    if (text.empty()) { return false; }
    for (const char c : text) {
        const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                     || (c >= '0' && c <= '9') || c == '_';
        if (!ok) { return false; }
    }
    return true;
}

// Vozvrashchaem nevladayushchij vid na podstroku ishodnoj stroki.
[[nodiscard]] std::string_view first_word(std::string_view text) {
    const std::size_t pos = text.find(' ');
    return pos == std::string_view::npos ? text : text.substr(0, pos);
}

}  // namespace

int main() {
    constexpr std::array<int, 5> data{1, 3, 5, 7, 9};
    std::println("total(array) = {}", total(data));

    const std::array<int, 3> more{2, 4, 6};
    std::println("total(other) = {}", total(more));

    std::println("'hero_1' valid?  {}", is_identifier("hero_1"));
    std::println("'bad name' valid? {}", is_identifier("bad name"));
    std::println("'' valid?         {}", is_identifier(""));

    const std::string line = "key value extra";
    std::println("first_word = {}", first_word(line));

    // Sborka stroki i sravnenie s C-strokoj.
    std::string built = "Hello";
    built += ", ";
    built += "world";
    const char* cstr = "Hello, world";
    std::println("built = '{}' (len {}), cstr equal? {}", built, built.size(),
                 built == cstr);

    std::cout << "arrays/strings done\n";
    return 0;
}
