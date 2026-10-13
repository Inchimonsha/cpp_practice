// m00_tooling/ex05_testing_doctest — mikro-frejmvork i unit-testy.
// Standart: C++23. Zapusk: ./run.ps1 m00_tooling/ex05_testing_doctest
#include <cstddef>
#include <functional>
#include <iostream>
#include <print>
#include <string>
#include <utility>
#include <vector>

namespace microtest {

struct Case {
    std::string name;
    std::function<void()> body;
};

inline std::vector<Case>& registry() {
    static std::vector<Case> cases;
    return cases;
}

inline int& failures() {
    static int count = 0;
    return count;
}

inline std::string& current_test() {
    static std::string name;
    return name;
}

struct Registrar {
    Registrar(std::string name, std::function<void()> body) {
        registry().push_back({std::move(name), std::move(body)});
    }
};

inline void expect(bool condition, const char* expr, const char* file, int line) {
    if (!condition) {
        ++failures();
        std::cout << "  FAIL [" << current_test() << "] " << expr
                  << "  (" << file << ':' << line << ")\n";
    }
}

inline int run_all() {
    const int total = static_cast<int>(registry().size());
    for (Case& c : registry()) {
        current_test() = c.name;
        const int before = failures();
        c.body();
        const bool ok = (failures() == before);
        std::println("{} {}", ok ? "PASSED" : "FAILED", c.name);
    }
    std::println("----\n{} tests, {} failed", total, failures());
    return failures() == 0 ? 0 : 1;
}

}  // namespace microtest

#define TEST(name)                                                             \
    static void name();                                                        \
    [[maybe_unused]] static microtest::Registrar reg_##name(#name, name);      \
    static void name()

#define EXPECT(cond) microtest::expect((cond), #cond, __FILE__, __LINE__)

namespace {
int add(int a, int b) { return a + b; }

int clamp_range(int value, int lo, int hi) {
    if (value < lo) { return lo; }
    if (value > hi) { return hi; }
    return value;
}

bool is_even(int value) { return value % 2 == 0; }
}  // namespace

TEST(adds_positive) { EXPECT(add(2, 2) == 4); }
TEST(adds_negative) { EXPECT(add(-2, -3) == -5); }
TEST(clamps_low) { EXPECT(clamp_range(-5, 0, 10) == 0); }
TEST(clamps_high) { EXPECT(clamp_range(99, 0, 10) == 10); }
TEST(keeps_inside) { EXPECT(clamp_range(7, 0, 10) == 7); }
TEST(even_numbers) { EXPECT(is_even(4)); EXPECT(!is_even(5)); }

int main() {
    return microtest::run_all();
}
