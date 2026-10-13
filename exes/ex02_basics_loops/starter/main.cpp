// ex02 — Base Defense: каркас (starter).
// Заполните TODO, следуя task.md. Подсказки — в hints.md.

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

        // TODO 1: switch по choice (см. task.md, п. 9).
        //   case 1: monster_hp = apply_damage(monster_hp, kHeroDamage);
        //   case 2: monster_hp = apply_damage(monster_hp,
        //                                    calculate_hit(kHeroDamage, true));
        //   case 3: hero_hp = clamp_hp(hero_hp + kHealAmount);
        //   case 4: running = false;
        //   default: печать «Неизвестная команда.»
        (void)choice;  // TODO: убрать, когда choice будет использован

        // TODO 2: если игрок вышел (running == false) — завершить итерацию
        //   (continue или структурно). Монстр не атакует в ход выхода.

        // TODO 3: если монстр ещё жив — он бьёт героя:
        //   hero_hp = apply_damage(hero_hp, kMonsterDamage);
    }

    // TODO 4: итог боя. Важен порядок проверок:
    //   мёртв герой   -> "Поражение..."
    //   мёртв монстр  -> "Победа!"
    //   оба живы      -> "Вы вышли из боя." (игрок выбрал команду 4)
    std::cout << "\n=== Бой окончен ===\n";
    return 0;
}