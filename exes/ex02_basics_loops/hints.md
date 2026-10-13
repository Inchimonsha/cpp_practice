# Подсказки: ex02 — Base Defense

Подсказки открывайте по мере необходимости — сначала попробуйте решить сами.

## Уровень 1. Структура действия (switch)

```cpp
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
```

## Уровень 2. Ход монстра

Монстр атакует, только если жив и если игрок не вышел:

```cpp
if (!running) {
    continue;
}
if (ex02::is_alive(monster_hp)) {
    hero_hp = ex02::apply_damage(hero_hp, ex02::kMonsterDamage);
}
```

## Уровень 3. Итог боя

Проверяйте в порядке «смерть игрока → смерть монстра → выход»:

```cpp
if (!ex02::is_alive(hero_hp)) {
    std::cout << "Поражение...\n";
} else if (!ex02::is_alive(monster_hp)) {
    std::cout << "Победа!\n";
} else {
    std::cout << "Вы вышли из боя.\n";
}
```

Добровольный выход оставляет обоих живыми → цикл прервался по `running` →
печатаем «Вы вышли из боя.», а не «Победа!».

## Уровень 4. Если бой не заканчивается

Проверьте условие цикла. `is_alive(x)` истинно при `x > 0`. Если вы забыли
применять урон монстра (`apply_damage`), монстр живёт вечно.