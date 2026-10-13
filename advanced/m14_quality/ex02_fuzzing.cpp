// m14_quality/ex02_fuzzing — mutacionnyy fuzzing s otsenki pokrytiya vetvey.
// Determinizovano fiksirovannym seed-om. Real'nye krahi lovyat ASan/UBSan.
// Standart: C++23. Zapusk: ./run.ps1 m14_quality/ex02_fuzzing
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <random>
#include <string>
#include <string_view>
#include <vector>

struct Coverage {
    std::array<bool, 8> branches{};
    int count() const {
        int total = 0;
        for (bool seen : branches) {
            total += seen ? 1 : 0;
        }
        return total;
    }
};

int target(std::string_view input, Coverage& coverage) {
    coverage.branches[0] = true;
    if (input.empty()) {
        coverage.branches[1] = true;
        return 0;
    }
    coverage.branches[2] = true;
    int total = 0;
    for (char symbol : input) {
        if (symbol >= '0' && symbol <= '9') {
            coverage.branches[3] = true;
            total += symbol - '0';
        } else if (symbol >= 'a' && symbol <= 'z') {
            coverage.branches[4] = true;
            total += 1;
        } else if (symbol >= 'A' && symbol <= 'Z') {
            coverage.branches[5] = true;
            total += 2;
        } else {
            coverage.branches[6] = true;
            total -= 1;
        }
    }
    if (input.size() > 3) {
        coverage.branches[7] = true;
        total *= 2;
    }
    return total;
}

int main() {
    const std::string alphabet = "abcXYZ019 .!";
    std::mt19937 generator(0xC0FFEEu);
    std::vector<std::string> corpus = {"", "a", "1", "Z9"};

    Coverage global;
    int crashes = 0;

    for (int iteration = 0; iteration < 20000; ++iteration) {
        std::string candidate = corpus[generator() % corpus.size()];
        const int mutations = 1 + static_cast<int>(generator() % 3);
        for (int step = 0; step < mutations; ++step) {
            const unsigned operation = generator() % 3;
            if (operation == 0 && !candidate.empty()) {
                candidate[generator() % candidate.size()] = alphabet[generator() % alphabet.size()];
            } else if (operation == 1) {
                candidate.push_back(alphabet[generator() % alphabet.size()]);
            } else if (operation == 2 && candidate.size() > 1) {
                candidate.erase(candidate.begin() + static_cast<std::ptrdiff_t>(
                                                       generator() % candidate.size()));
            }
        }

        Coverage local;
        try {
            static_cast<void>(target(candidate, local));
        } catch (...) {
            ++crashes;
        }

        bool new_coverage = false;
        for (std::size_t branch = 0; branch < global.branches.size(); ++branch) {
            if (local.branches[branch] && !global.branches[branch]) {
                global.branches[branch] = true;
                new_coverage = true;
            }
        }
        if (new_coverage) {
            corpus.push_back(candidate);
        }
    }

    std::println("fuzzing: 20000 iterations, corpus reached {}", corpus.size());
    std::println("branch coverage = {}/8", global.count());
    std::println("crashes = {}", crashes);

    const bool ok = crashes == 0 && global.count() >= 6;
    std::cout << "fuzzing done\n";
    return ok ? 0 : 1;
}
