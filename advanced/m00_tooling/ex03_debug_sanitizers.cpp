// m00_tooling/ex03_debug_sanitizers — sanitizers i proverki STL.
// Standart: C++23. Zapusk: ./run.ps1 m00_tooling/ex03_debug_sanitizers
#include <cstddef>
#include <iostream>
#include <print>
#include <vector>

namespace {

// Summa pervyh n elementov. V obeih vetvyah est' dostup po indeksu.
[[nodiscard]] long long sum_first_n(const std::vector<int>& data, std::size_t n) {
#ifdef DEMO_UB
    // Namernaya oshibka dlya demonstracii: <= vmesto < (vyhod za granicu).
    long long total = 0;
    for (std::size_t i = 0; i <= n; ++i) {
        total += data[i];  // pri i == data.size() eto UB
    }
    return total;
#else
    long long total = 0;
    const std::size_t limit = n < data.size() ? n : data.size();
    for (std::size_t i = 0; i < limit; ++i) {
        total += data[i];
    }
    return total;
#endif
}

}  // namespace

int main() {
    const std::vector<int> data{10, 20, 30};

#ifdef DEMO_UB
    std::println("DEMO_UB: chitaem za granicej - zhdyom assert ot STL");
#else
    std::println("safe mode");
#endif

    const long long total = sum_first_n(data, 3);
    std::println("sum_first_n(data, 3) = {}", total);

    return total == 60 ? 0 : 1;
}
