// ex02 — Base Defense: эталонное решение. C++20.
// Логика (battle.hpp) — в shared/ и покрыта тестами; здесь только ввод-вывод,
// выбор действия и выход из цикла.

#include <ex02/battle.hpp>

#include <iostream>

namespace {

void print_status(int hero_hp, int monster_hp) {
    std::cout << "\n[герой " << hero_hp << " HP] против [монстр "
              << monster_hp << " HP]\n";
}

void print_menu() {
    std::cout << "\nДействия:\n"
              << "  1 — атака\n"
              << "  2 — мощный удар\n"
              << "  3 — лечение (+20 HP)\n"
              << "  4 — выход\n"
              << "Ваш выбор: ";
}

int read_choice() {
    int choice = 0;
    std::cin >> choice;
    return choice;
}

}  // namespace

int main() {
    int hero_hp = ex02::kMaxHp;
    int monster_hp = ex02::kMaxHp;

    std::cout << "=== Base Defense ===\n";
    std::cout << "Вы защищаете базу от монстра!\n";

    bool running = true;
    while (running && ex02::is_alive(monster_hp) && ex02::is_alive(hero_hp)) {
        print_status(hero_hp, monster_hp);
        print_menu();

        const int choice = read_choice();
        switch (choice) {
            case 1:
                monster_hp = ex02::apply_damage(monster_hp, ex02::kHeroDamage);
                break;
            case 2:
                monster_hp = ex02::apply_damage(
                    monster_hp, ex02::calculate_hit(ex02::kHeroDamage, true));
                break;
            case 3:
                hero_hp = ex02::clamp_hp(hero_hp + ex02::kHealAmount);
                break;
            case 4:
                running = false;
                break;
            default:
                std::cout << "Неизвестная команда.\n";
                break;
        }

        if (!running) {
            continue;  // в ход выхода монстр не атакует
        }

        if (ex02::is_alive(monster_hp)) {
            hero_hp = ex02::apply_damage(hero_hp, ex02::kMonsterDamage);
        }
    }

    std::cout << "\n=== Бой окончен ===\n";
    if (!ex02::is_alive(hero_hp)) {
        std::cout << "Поражение...\n";
    } else if (!ex02::is_alive(monster_hp)) {
        std::cout << "Победа!\n";
    } else {
        std::cout << "Вы вышли из боя.\n";
    }
    return 0;
}