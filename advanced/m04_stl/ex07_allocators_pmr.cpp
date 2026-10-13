// m04_stl/ex07_allocators_pmr — allokatory i std::pmr.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex07_allocators_pmr
#include <array>
#include <cstddef>
#include <iostream>
#include <memory_resource>
#include <print>
#include <string>
#include <vector>

int main() {
    std::println("monotonic_buffer_resource (arena):");
    std::array<std::byte, 4096> buffer{};
    std::pmr::monotonic_buffer_resource arena{buffer.data(), buffer.size()};
    {
        std::pmr::vector<int> values{&arena};
        for (int i = 0; i < 10; ++i) { values.push_back(i * i); }
        std::println("  size={} capacity={}", values.size(), values.capacity());

        std::pmr::string text{"allocated in arena", &arena};
        std::println("  string = '{}' (len={})", text, text.size());
    }
    arena.release();  // osvobodit' vsyu arenu srazu
    std::println("  arena released");

    std::println("unsynchronized_pool_resource (pool):");
    std::pmr::unsynchronized_pool_resource pool;
    {
        std::pmr::vector<std::pmr::string> lines{&pool};
        lines.emplace_back("first line");
        lines.emplace_back("second line");
        lines.emplace_back("third line");
        std::println("  lines={}", lines.size());
        for (const auto& line : lines) { std::println("    '{}'", line); }
    }

    std::println("default new_delete_resource:");
    std::pmr::vector<int> standard{std::pmr::new_delete_resource()};
    standard.push_back(42);
    std::println("  size={} front={}", standard.size(), standard.front());

    std::cout << "pmr done\n";
    return 0;
}
