// m00_tooling/ex01_workflow — odin fayl, odin zapusk.
// Standart: C++23. Zapusk: ./run.ps1 m00_tooling/ex01_workflow
#include <cstddef>
#include <format>
#include <print>
#include <string>

int main() {
    std::println("standard = {} (C++23 == 202302L)", __cplusplus);
    std::println("compiler = {}", __VERSION__);
    std::println("platform: int={} long={} size_t={} double={} void*={}", sizeof(int),
                 sizeof(long), sizeof(std::size_t), sizeof(double), sizeof(void*));

    const std::string line = std::format("formatted: {} + {} = {}", 2, 3, 2 + 3);
    std::println("{}", line);

    return __cplusplus >= 202302L ? 0 : 1;
}
