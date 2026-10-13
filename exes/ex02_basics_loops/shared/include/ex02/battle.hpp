// battle.hpp — логика боя ex02. Контракт для solution и tests.
// Все функции «чистые» (без консоли и случайности) — их можно тестировать.

#pragma once

namespace ex02 {

// Максимум и минимум здоровья бойца.
constexpr int kMaxHp = 100;
constexpr int kMinHp = 0;

// Обычный урон героя, урон монстра, лечение одним действием.
constexpr int kHeroDamage = 25;
constexpr int kMonsterDamage = 10;
constexpr int kHealAmount = 20;

// Ограничивает значение hp диапазоном [kMinHp, kMaxHp].
int clamp_hp(int hp);

// Наносит damage текущему hp и возвращает новое (не ниже kMinHp).
int apply_damage(int hp, int damage);

// Жив ли боец (hp > 0).
bool is_alive(int hp);

// Урон с учётом «мощного удара»: критический удар удваивает damage.
int calculate_hit(int base_damage, bool critical);

}  // namespace ex02