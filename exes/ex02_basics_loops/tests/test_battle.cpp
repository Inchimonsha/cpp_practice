// Тесты логики боя ex02. Проверяют контракт shared/battle.hpp.

#include <ex02/battle.hpp>

#include <cpp_course/test.hpp>

using ex02::apply_damage;
using ex02::calculate_hit;
using ex02::clamp_hp;
using ex02::is_alive;
using ex02::kHealAmount;
using ex02::kMaxHp;

CPP_COURSE_TEST("clamp_hp: значения внутри диапазона не меняются") {
    CHECK_EQ(clamp_hp(50), 50);
    CHECK_EQ(clamp_hp(0), 0);
    CHECK_EQ(clamp_hp(kMaxHp), kMaxHp);
}

CPP_COURSE_TEST("clamp_hp: верхняя граница") {
    CHECK_EQ(clamp_hp(150), kMaxHp);
    CHECK_EQ(clamp_hp(kMaxHp + 1), kMaxHp);
}

CPP_COURSE_TEST("clamp_hp: нижняя граница") {
    CHECK_EQ(clamp_hp(-5), 0);
    CHECK_EQ(clamp_hp(-100), 0);
}

CPP_COURSE_TEST("apply_damage: уменьшает здоровье") {
    CHECK_EQ(apply_damage(80, 25), 55);
}

CPP_COURSE_TEST("apply_damage: не опускается ниже нуля") {
    CHECK_EQ(apply_damage(10, 40), 0);
    CHECK_EQ(apply_damage(5, 100), 0);
}

CPP_COURSE_TEST("apply_damage: лечение невозможно этой функцией") {
    CHECK_EQ(apply_damage(10, -20), 30);  // отрицательный урон = лечение
}

CPP_COURSE_TEST("is_alive: только hp > 0") {
    CHECK(!is_alive(0));
    CHECK(!is_alive(-1));
    CHECK(is_alive(1));
    CHECK(is_alive(kMaxHp));
}

CPP_COURSE_TEST("calculate_hit: обычный и критический удар") {
    CHECK_EQ(calculate_hit(25, false), 25);
    CHECK_EQ(calculate_hit(25, true), 50);
    CHECK_EQ(calculate_hit(0, true), 0);
}

CPP_COURSE_TEST("большой урон после лечения не выходит за пределы") {
    int hero_hp = apply_damage(kMaxHp, 30);            // 70
    hero_hp = hero_hp + kHealAmount;                   // 90
    hero_hp = clamp_hp(hero_hp);                       // 90
    hero_hp = apply_damage(hero_hp, 200);              // 0
    CHECK_EQ(hero_hp, 0);
    CHECK(!is_alive(hero_hp));
}