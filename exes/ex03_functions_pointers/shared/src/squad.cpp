// squad.cpp — реализация логики отряда ex03.

#include <ex03/squad.hpp>

#include <algorithm>

namespace ex03 {

void heal(Fighter& f, int amount) {
    if (f.hp <= 0) {
        return;  // не воскрешаем
    }
    f.hp = std::min(f.hp + amount, f.max_hp);
}

bool try_hit(Fighter* target, int damage) {
    if (target == nullptr) {
        return false;
    }
    target->hp = std::max(target->hp - damage, 0);
    return target->hp > 0;
}

Fighter* find_weakest(std::vector<Fighter>& squad) {
    Fighter* weakest = nullptr;
    for (Fighter& fighter : squad) {
        if (fighter.hp <= 0) {
            continue;  // мёртвых не считаем
        }
        if (weakest == nullptr || fighter.hp < weakest->hp) {
            weakest = &fighter;
        }
    }
    return weakest;
}

int total_damage(const std::vector<Fighter>& squad) {
    int sum = 0;
    for (const Fighter& fighter : squad) {
        if (fighter.hp > 0) {
            sum += fighter.damage;
        }
    }
    return sum;
}

}  // namespace ex03