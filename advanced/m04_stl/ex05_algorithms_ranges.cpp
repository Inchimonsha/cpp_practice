// m04_stl/ex05_algorithms_ranges — algoritmy i std::ranges.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex05_algorithms_ranges
#include <algorithm>
#include <iostream>
#include <numeric>
#include <print>
#include <string>
#include <vector>

namespace {

struct Person {
    std::string name;
    int score{};
};

}  // namespace

int main() {
    std::vector<Person> people{
        {"alice", 80}, {"bob", 95}, {"carol", 70}, {"dave", 95}};

    // Sortiruem po score po ubyvaniyu (proekciya na pole).
    std::ranges::stable_sort(people, std::greater{}, &Person::score);
    std::println("sorted by score desc:");
    for (const Person& p : people) {
        std::println("  {}: {}", p.name, p.score);
    }

    // Poisk po imeni cherez proekciyu.
    const auto it = std::ranges::find(people, std::string("carol"), &Person::name);
    std::println("find carol: {}", it != people.end() ? it->name : "not found");

    // Sobrat' imena.
    std::vector<std::string> names;
    std::ranges::transform(people, std::back_inserter(names), &Person::name);
    std::print("names:");
    for (const std::string& n : names) { std::print(" {}", n); }
    std::print("\n");

    // Summa ballov.
    const int total = std::accumulate(
        people.begin(), people.end(), 0,
        [](int acc, const Person& p) { return acc + p.score; });
    std::println("total score = {}", total);

    // Predikaty.
    const bool any_top = std::ranges::any_of(people, [](const Person& p) { return p.score > 90; });
    const bool all_pass = std::ranges::all_of(people, [](const Person& p) { return p.score >= 60; });
    const auto high = std::ranges::count_if(people, [](const Person& p) { return p.score >= 90; });
    std::println("any>90={} all>=60={} count>=90={}", any_top, all_pass, high);

    std::cout << "algorithms/ranges done\n";
    return (people.front().name == "bob" && total == 340) ? 0 : 1;
}
