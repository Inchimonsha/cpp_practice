// m04_stl/ex02_associative_containers — associativnye kontejnery.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex02_associative_containers
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <print>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>

namespace {

struct GridPos {
    int x{};
    int y{};
    bool operator==(const GridPos& other) const { return x == other.x && y == other.y; }
};

struct GridPosHash {
    std::size_t operator()(const GridPos& p) const {
        const std::size_t hx = std::hash<int>{}(p.x);
        const std::size_t hy = std::hash<int>{}(p.y);
        return hx ^ (hy << 1);
    }
};

}  // namespace

int main() {
    std::println("ordered map:");
    std::map<std::string, int> scores;
    scores["carol"] = 30;
    scores["alice"] = 10;
    scores.insert_or_assign("bob", 20);
    for (const auto& [name, score] : scores) {
        std::println("  {} -> {}", name, score);
    }
    std::println("  contains(bob) = {}", scores.contains("bob"));
    std::println("  contains(dave) = {}", scores.contains("dave"));

    std::println("set (unique):");
    std::set<int> unique{3, 1, 2, 3, 1};
    std::print("  sorted unique:");
    for (int v : unique) { std::print(" {}", v); }
    std::print("  size={}\n", unique.size());

    std::println("unordered_map with custom key:");
    std::unordered_map<GridPos, int, GridPosHash> grid;
    grid[{0, 0}] = 1;
    grid[{2, 3}] = 42;
    std::println("  at(2,3) = {}", grid.at(GridPos{2, 3}));
    std::println("  size = {}", grid.size());

    std::println("transparent lookup:");
    std::map<std::string, int, std::less<>> tmap;
    tmap["key"] = 99;
    const std::string_view probe = "key";
    const auto it = tmap.find(probe);   // bez konversii v std::string
    std::println("  find(string_view) = {}", it != tmap.end() ? it->second : -1);

    std::cout << "associative containers done\n";
    return (scores.at("bob") == 20 && unique.size() == 3 && grid.at(GridPos{2, 3}) == 42)
               ? 0 : 1;
}
