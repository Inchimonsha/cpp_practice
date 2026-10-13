// m01_syntax/ex01_types_literals — tipy i literaly.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex01_types_literals
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <print>
#include <string>
#include <string_view>

namespace {

template <typename T>
void describe(std::string_view name) {
    std::println("{:<10} bytes={:<3} min={} max={}",
                 name, sizeof(T),
                 std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
}

void show_limits() {
    describe<int>("int");
    describe<std::uint32_t>("uint32_t");
    describe<std::int64_t>("int64_t");
    describe<double>("double");
}

}  // namespace

int main() {
    static_assert(sizeof(char) == 1, "char dolzhen byt' 1 bajt");
    static_assert(sizeof(std::uint32_t) == 4);

    show_limits();

    constexpr int          dec = 1'000'000;
    constexpr unsigned     hex = 0xFFu;
    constexpr unsigned     bin = 0b1010'1111u;
    constexpr long long    big = 9'000'000'000LL;
    constexpr double       pi  = 3.14159;
    constexpr float        f   = 2.5f;
    constexpr char         ch  = 'A';
    const std::string      raw = R"(C:\path\no\escape)";
    const std::u8string    utf8 = u8"привет";

    std::println("dec={} hex={} bin={} big={}", dec, hex, bin, big);
    std::println("pi={} f={} ch={}", pi, f, ch);
    std::println("raw={}", raw);
    std::println("utf8 bytes={}", utf8.size());

    // Yavnoe suzhenie vmesto neyavnogo (chisto pod -Wconversion).
    const int narrow = static_cast<int>(big % 1000);
    std::println("big % 1000 = {}", narrow);

    std::cout << "types map done\n";
    return 0;
}
