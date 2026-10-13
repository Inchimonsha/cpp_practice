// ex03 — Squad Combat Simulator: каркас (starter).
// Заполните TODO, следуя task.md. Подсказки — в hints.md.

#include <ex03/squad.hpp>

#include <iostream>

namespace {

// Печатает состояние отряда. Принимает const& — только чтение.
void print_squad(const std::vector<ex03::Fighter>& squad) {
    for (const ex03::Fighter& f : squad) {
        std::cout << "  " << f.name << ": HP " << f.hp << "/" << f.max_hp
                  << " (урон " << f.damage << ")\n";
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

    // TODO 1: «раунд» — 3 шага:
    //   а) найти самого слабого живого: наименьший HP;
    //   б) если он есть — ударить на 15 (try_hit);
    //   в) подлечить двух первых бойцов на 10 (heal).
    //   Слабым может оказаться и 2-й, и 3-й боец — это нормально.

    // TODO 2: вывести на экран:
    //   силу отряда (total_damage),
    //   имя самого слабого сейчас (или "нет живых"),
    //   итоговое состояние отряда (print_squad).

    return 0;
}