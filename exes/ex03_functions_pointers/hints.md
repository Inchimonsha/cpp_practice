# Подсказки: ex03 — Squad Combat Simulator

Подсказки открывайте по мере необходимости — сначала попробуйте решить сами.

## Уровень 1. heal (ссылка)

```cpp
void heal(Fighter& f, int amount) {
    if (f.hp <= 0) {
        return;  // мёртвых не лечим
    }
    f.hp = std::min(f.hp + amount, f.max_hp);  // <algorithm>
}
```

Ссылка нужна, потому что мы изменяем переданного бойца, а не копию.

## Уровень 2. try_hit (указатель)

```cpp
bool try_hit(Fighter* target, int damage) {
    if (target == nullptr) {
        return false;
    }
    target->hp = std::max(target->hp - damage, 0);
    return target->hp > 0;
}
```

`target->hp` эквивалентно `(*target).hp`.

## Уровень 3. find_weakest (указатель против nullptr)

```cpp
Fighter* find_weakest(std::vector<Fighter>& squad) {
    Fighter* weakest = nullptr;
    for (Fighter& fighter : squad) {
        if (fighter.hp <= 0) {
            continue;
        }
        if (weakest == nullptr || fighter.hp < weakest->hp) {
            weakest = &fighter;
        }
    }
    return weakest;
}
```

Первое условие защищает от разыменования `nullptr`: сравниваем только при
`weakest != nullptr`.

## Уровень 4. total_damage (const& и range-for)

```cpp
int total_damage(const std::vector<Fighter>& squad) {
    int sum = 0;
    for (const Fighter& fighter : squad) {
        if (fighter.hp > 0) {
            sum += fighter.damage;
        }
    }
    return sum;
}
```

`const &` — читаем, не изменяем.

## Уровень 5. Демонстрация в main

```cpp
Fighter* weakest = find_weakest(squad);
if (weakest != nullptr) {
    (void)try_hit(weakest, 15);
}
heal(squad[0], 10);
heal(squad[1], 10);
```

Обратите внимание: `find_weakest` вернул **указатель** на элемент вектора;
удары и лечение модифицируют оригинал, а не копию.