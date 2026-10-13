// m14_quality/ex01_property_based — svoystva vmesto primerov + shrinking.
// Determinizovano fiksirovannym seed-om GPN.
// Standart: C++23. Zapusk: ./run.ps1 m14_quality/ex01_property_based
#include <algorithm>
#include <iostream>
#include <print>
#include <random>
#include <vector>

std::vector<int> reverse_copy(const std::vector<int>& input) {
    return std::vector<int>(input.rbegin(), input.rend());
}

int main() {
    std::mt19937 generator(12345);
    std::uniform_int_distribution<int> length_distribution(0, 20);
    std::uniform_int_distribution<int> value_distribution(-1000, 1000);

    const auto random_vector = [&]() {
        std::vector<int> values(static_cast<std::size_t>(length_distribution(generator)));
        for (int& value : values) {
            value = value_distribution(generator);
        }
        return values;
    };

    bool reverse_ok = true;
    for (int case_index = 0; case_index < 200 && reverse_ok; ++case_index) {
        const std::vector<int> values = random_vector();
        reverse_ok = reverse_copy(reverse_copy(values)) == values;
    }
    std::println("property reverse-involution: {}",
                 reverse_ok ? "passed (200 cases)" : "FAILED");

    bool sorted_ok = true;
    for (int case_index = 0; case_index < 200 && sorted_ok; ++case_index) {
        std::vector<int> values = random_vector();
        std::sort(values.begin(), values.end());
        sorted_ok = std::is_sorted(values.begin(), values.end());
    }
    std::println("property sort-ordered: {}",
                 sorted_ok ? "passed (200 cases)" : "FAILED");

    std::uniform_int_distribution<int> candidate_distribution(0, 1000);
    const auto fails_property = [](int value) { return !(value < 100); };

    int counterexample = 0;
    bool found = false;
    for (int attempt = 0; attempt < 1000 && !found; ++attempt) {
        const int candidate = candidate_distribution(generator);
        if (fails_property(candidate)) {
            counterexample = candidate;
            found = true;
        }
    }

    bool shrunk_progress = true;
    while (shrunk_progress) {
        shrunk_progress = false;
        if (const int half = counterexample / 2; fails_property(half)) {
            counterexample = half;
            shrunk_progress = true;
            continue;
        }
        if (fails_property(counterexample - 1)) {
            counterexample -= 1;
            shrunk_progress = true;
        }
    }

    std::println("property value < 100: {}",
                 found ? "FAILED, shrunk counterexample = " + std::to_string(counterexample)
                       : "passed");
    std::println("property based done");

    const bool ok = reverse_ok && sorted_ok && found && counterexample == 100;
    return ok ? 0 : 1;
}
