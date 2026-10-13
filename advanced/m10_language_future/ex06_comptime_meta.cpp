// m10_language_future/ex06_comptime_meta — metaprogrammirovanie vremeni kompilyacii.
// C++23/26: consteval, if consteval, constexpr-vychisleniya.
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>
#include <string_view>

constexpr std::uint64_t fnv1a(std::string_view text) {
    std::uint64_t hash = 1469598103934665603ull;
    for (const char ch : text) {
        hash ^= static_cast<unsigned char>(ch);
        hash *= 1099511628211ull;
    }
    return hash;
}

consteval std::uint64_t compile_time_hash(std::string_view text) {
    return fnv1a(text);
}

consteval bool is_prime(const int n) {
    if (n < 2) {
        return false;
    }
    for (int d = 2; d * d <= n; ++d) {
        if (n % d == 0) {
            return false;
        }
    }
    return true;
}

template <std::size_t N>
consteval std::array<int, N> first_primes() {
    std::array<int, N> result{};
    int candidate = 2;
    std::size_t found = 0;
    while (found < N) {
        if (is_prime(candidate)) {
            result[found] = candidate;
            ++found;
        }
        ++candidate;
    }
    return result;
}

constexpr int evaluate(const int x) {
    if consteval {
        return x * 2;
    } else {
        return x * 3;
    }
}

constexpr int color_of(const std::string_view name) {
    switch (fnv1a(name)) {
        case fnv1a("red"):
            return 0xFF0000;
        case fnv1a("green"):
            return 0x00FF00;
        case fnv1a("blue"):
            return 0x0000FF;
        default:
            return -1;
    }
}

static_assert(compile_time_hash("send") != compile_time_hash("recv"));
static_assert(fnv1a("red") == fnv1a("red"));
static_assert(is_prime(19) && !is_prime(21));
static_assert(first_primes<8>()[7] == 19);
static_assert(evaluate(5) == 10);

int main() {
    std::println("fnv1a(\"send\") = {}", fnv1a("send"));

    constexpr auto primes = first_primes<8>();
    std::print("first_primes<8> =");
    for (const int p : primes) {
        std::print(" {}", p);
    }
    std::print("\n");

    std::println("color_of(\"green\") = {:#x}", color_of("green"));
    std::println("color_of(\"magenta\") = {}", color_of("magenta"));

    constexpr int compile_time_value = evaluate(5);
    volatile int seed = 5;
    const int runtime_value = evaluate(seed);
    std::println("evaluate compile-time = {}, runtime = {}", compile_time_value,
                 runtime_value);

    const bool ok = compile_time_value == 10 && runtime_value == 15 &&
                    color_of("green") == 0x00FF00 && color_of("magenta") == -1 &&
                    primes.front() == 2 && primes.back() == 19;
    std::cout << "comptime meta done\n";
    return ok ? 0 : 1;
}
