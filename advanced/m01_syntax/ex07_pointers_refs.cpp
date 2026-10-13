// m01_syntax/ex07_pointers_refs — ukazateli i ssylki.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex07_pointers_refs
#include <cstddef>
#include <iostream>
#include <print>
#include <string>

namespace {

void scale(double& value, double k) { value *= k; }

void print_label(const std::string& label) { std::println("label: {}", label); }

// Ustojchivo k nullptr: vozvrashchaet fallback, esli ukazatelya net.
[[nodiscard]] int optional_read(const int* p, int fallback) {
    return p != nullptr ? *p : fallback;
}

[[nodiscard]] int sum_array(const int* data, std::size_t count) {
    int total = 0;
    for (const int* it = data; it != data + count; ++it) {
        total += *it;
    }
    return total;
}

struct Player {
    int hp{100};
    int level{1};
};

}  // namespace

int main() {
    double damage = 40.0;
    scale(damage, 1.5);
    std::println("damage after scale = {}", damage);

    print_label("boss");

    int target = 7;
    std::println("optional_read(&target) = {}", optional_read(&target, -1));
    std::println("optional_read(nullptr) = {}", optional_read(nullptr, -1));

    const int data[5]{2, 4, 6, 8, 10};
    std::println("sum_array = {}", sum_array(data, 5));

    // const int* : dannye nel'zya menyat' cherez ukazatel'.
    const int* to_const = &target;
    // int* const : sam ukazatel' nel'zya perevesti.
    int* const fixed = &target;
    *fixed = 9;
    std::println("*to_const = {}, *fixed = {}", *to_const, *fixed);

    // Ukazatel' na chlen klassa.
    const int Player::* member = &Player::hp;
    Player hero;
    std::println("hero.*member = {}", hero.*member);

    std::cout << "pointers/refs done\n";
    return 0;
}
