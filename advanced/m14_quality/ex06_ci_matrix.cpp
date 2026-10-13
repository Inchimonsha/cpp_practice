// m14_quality/ex06_ci_matrix — matrica sborki CI i generaciya GitHub Actions.
// Determinizovano (pravila podderzhki zhdestko zadany).
// Standart: C++23. Zapusk: ./run.ps1 m14_quality/ex06_ci_matrix
#include <iostream>
#include <print>
#include <string>
#include <vector>

struct Job {
    std::string os;
    std::string compiler;
    std::string standard;
};

bool supported(const Job& job) {
    if (job.compiler == "msvc-19") {
        return job.os == "windows-latest";
    }
    if (job.compiler == "gcc-13" && job.os == "macos-latest") {
        return false;
    }
    return true;
}

bool passes(const Job& job) {
    return !(job.os == "windows-latest" && job.compiler == "clang-17" &&
             job.standard == "c++23");
}

int main() {
    const std::vector<std::string> systems = {"ubuntu-latest", "windows-latest", "macos-latest"};
    const std::vector<std::string> compilers = {"gcc-13", "clang-17", "msvc-19"};
    const std::vector<std::string> standards = {"c++20", "c++23"};

    std::vector<Job> jobs;
    for (const std::string& os : systems) {
        for (const std::string& compiler : compilers) {
            for (const std::string& standard : standards) {
                jobs.push_back(Job{os, compiler, standard});
            }
        }
    }

    int passed = 0;
    int failed = 0;
    int skipped = 0;
    std::println("ci matrix ({} combinations)", jobs.size());
    for (const Job& job : jobs) {
        if (!supported(job)) {
            ++skipped;
            std::println("[skip] {:<14} {} {} -> not supported", job.os, job.compiler,
                         job.standard);
            continue;
        }
        if (passes(job)) {
            ++passed;
            std::println("[run ] {:<14} {} {} -> pass", job.os, job.compiler, job.standard);
        } else {
            ++failed;
            std::println("[run ] {:<14} {} {} -> fail", job.os, job.compiler, job.standard);
        }
    }

    std::println("summary: passed = {}, failed = {}, skipped = {}", passed, failed, skipped);

    std::println("strategy yaml:");
    std::println("  matrix:");
    std::println("    os: [ubuntu-latest, windows-latest, macos-latest]");
    std::println("    compiler: [gcc-13, clang-17, msvc-19]");
    std::println("    standard: [c++20, c++23]");

    const bool ok = jobs.size() == systems.size() * compilers.size() * standards.size() &&
                    passed == 11 && failed == 1 && skipped == 6;
    std::cout << "ci matrix done\n";
    return ok ? 0 : 1;
}
