// squad.hpp — логика отряда ex03. Контракт для solution и tests.
// Никакой консоли и случайности — функции «чистые».

#pragma once

#include <string>
#include <vector>

namespace ex03 {

// Один боец отряда.
struct Fighter {
    std::string name;   // имя (ex04 — работа со строками)
    int hp    = 100;
    int max_hp = 100;
    int damage = 10;
};

// Восстанавливает amount HP бойцу с пределом max_hp.
// Мёртвых (hp == 0) НЕ воскрешает.
void heal(Fighter& f, int amount);

// Наносит damage цели. Цель задана указателем: можно передать nullptr.
// Уменьшает HP, но не ниже 0. Возвращает: жив ли боец после удара.
bool try_hit(Fighter* target, int damage);

// Находит живого бойца с наименьшим HP. Мёртвых игнорирует.
// Пустой отряд или «все мертвы» → nullptr.
Fighter* find_weakest(std::vector<Fighter>& squad);

// Сумма урона по-живущих бойцов отряда.
int total_damage(const std::vector<Fighter>& squad);

}  // namespace ex03