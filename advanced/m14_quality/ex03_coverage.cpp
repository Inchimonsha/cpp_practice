// m14_quality/ex03_coverage — pokrytie koda (instrumentirovaniye vruchnuyu).
// Real'noe izmereniye cherez gcov: g++ --coverage ... && gcov (sm. .md).
// Standart: C++23. Zapusk: ./run.ps1 m14_quality/ex03_coverage
#include <iostream>
#include <map>
#include <print>
#include <string>
#include <vector>

struct Coverage {
    std::map<std::string, bool> branch_hits;
    std::map<std::string, bool> function_hits;
};

const std::vector<std::string>& all_branches() {
    static const std::vector<std::string> branches{"negative", "fail", "pass", "excellent"};
    return branches;
}

const std::vector<std::string>& all_functions() {
    static const std::vector<std::string> functions{"clamp_score", "grade"};
    return functions;
}

int hit_count(const std::vector<std::string>& names, const std::map<std::string, bool>& hits) {
    int total = 0;
    for (const std::string& name : names) {
        if (hits.count(name) != 0) {
            ++total;
        }
    }
    return total;
}

int clamp_score(int score, Coverage& coverage) {
    coverage.function_hits["clamp_score"] = true;
    if (score < 0) {
        return 0;
    }
    if (score > 100) {
        return 100;
    }
    return score;
}

int grade(int score, Coverage& coverage) {
    coverage.function_hits["grade"] = true;
    score = clamp_score(score, coverage);
    if (score == 0) {
        coverage.branch_hits["negative"] = true;
        return -1;
    }
    if (score < 50) {
        coverage.branch_hits["fail"] = true;
        return 2;
    }
    if (score < 75) {
        coverage.branch_hits["pass"] = true;
        return 3;
    }
    coverage.branch_hits["excellent"] = true;
    return 5;
}

int main() {
    Coverage coverage;
    static_cast<void>(grade(-1, coverage));
    static_cast<void>(grade(30, coverage));
    static_cast<void>(grade(60, coverage));

    const int functions_hit = hit_count(all_functions(), coverage.function_hits);
    const int functions_total = static_cast<int>(all_functions().size());
    const int branches_hit = hit_count(all_branches(), coverage.branch_hits);
    const int branches_total = static_cast<int>(all_branches().size());

    std::print("uncovered branches:");
    for (const std::string& name : all_branches()) {
        if (coverage.branch_hits.count(name) == 0) {
            std::print(" {}", name);
        }
    }
    std::println("");

    std::println("function coverage = {}% ({}/{})", functions_hit * 100 / functions_total,
                 functions_hit, functions_total);
    std::println("branch coverage = {}% ({}/{})", branches_hit * 100 / branches_total,
                 branches_hit, branches_total);

    const bool ok = functions_hit == functions_total && branches_hit == 3 && branches_total == 4;
    std::cout << "coverage done\n";
    return ok ? 0 : 1;
}
