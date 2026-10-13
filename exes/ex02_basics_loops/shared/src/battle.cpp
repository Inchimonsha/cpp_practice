// battle.cpp — реализация логики боя ex02.

#include <ex02/battle.hpp>

namespace ex02 {

int clamp_hp(int hp) {
    if (hp < kMinHp) {
        return kMinHp;
    }
    if (hp > kMaxHp) {
        return kMaxHp;
    }
    return hp;
}

int apply_damage(int hp, int damage) {
    return clamp_hp(hp - damage);
}

bool is_alive(int hp) {
    return hp > kMinHp;
}

int calculate_hit(int base_damage, bool critical) {
    if (critical) {
        return base_damage * 2;
    }
    return base_damage;
}

}  // namespace ex02