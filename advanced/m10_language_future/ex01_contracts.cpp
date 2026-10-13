// m10_language_future/ex01_contracts — kontrakty (C++26).
// Standart: C++26. Zapusk: ./run.ps1 m10_language_future/ex01_contracts -Std c++26
#include <iostream>
#include <numeric>
#include <print>
#include <vector>

#if defined(__cpp_contracts)
#  define PRE(cond) pre(cond)
#  define POST(cond) post(cond)
#  define CONTRACT_ASSERT(cond) contract_assert(cond)
#else
#  define PRE(cond)
#  define POST(cond)
#  define CONTRACT_ASSERT(cond) ((void)0)
#endif

int next_id(const int current)
    PRE(current >= 0)
    POST(r : r == current + 1)
{
    return current + 1;
}

double mean(const std::vector<int>& values)
    PRE(!values.empty())
{
    CONTRACT_ASSERT(values.size() > 0);
    const int sum = std::accumulate(values.begin(), values.end(), 0);
    return static_cast<double>(sum) / static_cast<double>(values.size());
}

int checked_div(const int numerator, const int denominator)
    PRE(denominator != 0)
    PRE(numerator >= 0)
{
    return numerator / denominator;
}

int main() {
#if defined(__cpp_contracts)
    std::println("__cpp_contracts = {}", __cpp_contracts);
#else
    std::println("contracts unavailable: fallback bez proverok");
#endif

    const int id = next_id(41);
    const double avg = mean({2, 4, 6});
    const int quotient = checked_div(42, 7);

    std::println("next_id(41) = {}", id);
    std::println("mean({{2,4,6}}) = {}", avg);
    std::println("checked_div(42,7) = {}", quotient);

    std::cout << "contracts done\n";
    return (id == 42 && mean({2, 4, 6}) == 4.0 && checked_div(42, 7) == 6) ? 0 : 1;
}
