// ex03 — Squad Combat Simulator: эталонное решение. C++20.
// Логика — в shared/ (squad.hpp); здесь только демонстрация.

#include <ex03/squad.hpp>

#include <iostream>

namespace {

void print_squad(const std::vector<ex03::Fighter>& squad) {
    for (const ex03::Fighter& f : squad) {
        std::cout << "  " << f.name << ": HP " << f.hp << "/" << f.max_hp
                  << " (урон " << f.damage << ")\n";
    }
}

void describe(std::vector<ex03::Fighter>& squad) {
    const ex03::Fighter* weakest = ex03::find_weakest(squad);
    std::cout << "Сила отряда: " << ex03::total_damage(squad) << '\n';
    if (weakest != nullptr) {
        std::cout << "Самый слабый: " << weakest->name << " (HP " << weakest->hp
                  << ")\n";
    } else {
        std::cout << "Самый слабый: нет живых\n";
    }
}

}  // namespace

int main() {
    std::vector<ex03::Fighter> squad{
        {"Mage", 60, 100, 25},
        {"Warrior", 90, 100, 20},
        {"Rogue", 30, 60, 35},
    };

    std::cout << "=== Squad Combat Simulator ===\n";
    print_squad(squad);
    std::cout << "--- раунд ---\n";

    ex03::Fighter* weakest = ex03::find_weakest(squad);
    if (weakest != nullptr) {
        std::cout << "Бьём самого слабого: " << weakest->name << '\n';
        (void)ex03::try_hit(weakest, 15);
    }

    for (int i = 0; i < 2 && i < static_cast<int>(squad.size()); ++i) {
        ex03::heal(squad[static_cast<std::size_t>(i)], 10);
    }

    describe(squad);
    print_squad(squad);
    return 0;
}