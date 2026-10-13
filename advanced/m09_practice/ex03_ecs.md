# ex03_ecs — Entity-Component-System (SoA)

## Теория

ECS разделяет данные и поведение:

- **Entity** — просто идентификатор (число), без данных.
- **Component** — «сырые» данные (позиция, скорость, здоровье), без логики.
- **System** — функция, обрабатывающая все сущности, у которых есть нужный набор
  компонентов.

Данные хранятся **раздельно по типам** (struct of arrays): система «движение»
читает только `Position` и `Velocity` — соседние по памяти, без лишних полей. Это
даёт локальность (см. m07) и гибкую композицию без наследования.

Типы компонентов задаются на этапе компиляции: реестр параметризуется их пакетом,
а «представление» (view) выбирает сущности с нужными компонентами свёрткой
`(has<Cs>(e) && ...)`.

## Синтаксис

```cpp
struct Position { float x, y; };
struct Velocity { float vx, vy; };

Registry<Position, Velocity, Health> reg;
Entity e = reg.create();
reg.add(e, Position{0, 0});
reg.add(e, Velocity{1, 0.5f});

reg.each<Position, Velocity>([](Entity, Position& p, Velocity& v) {
    p.x += v.vx;
});
```

## Подводные камни

- Такой реестр по типам фиксирован на компиляцию; динамические компоненты требуют
  стирания типов (см. ex05).
- Плотные id + векторы-флаги просты, но удаление сущностей оставляет «дыры»;
  production-ECS применяет swap-remove и поколения (`generation`), чтобы
  переиспользовать id безопасно.
- Держите компоненты «плоскими» (POD); избегайте указателей и тяжёлых типов.
- Итерацию лучше делать по одному компоненту (по архетипу), а не по всем id — иначе
  теряется локальность.
- Порядок обхода влияет на детерминизм — фиксируйте его.

## Задача

Реализовать `Registry<...>` с `create/add/has/get/each` и компонентами
`Position`, `Velocity`, `Health`. Система движения сдвигает только сущности с
`Position`+`Velocity` (`dt = 2`); система урона уменьшает `Health`. Проверить
фильтрацию (сущность без `Velocity` не двигается; сущность только с `Health`
видна системе урона).

## Где применяется

- Игровые движки (Unity DOTS, EnTT, flecs), симуляции, обработка сущностей с
  большим количеством объектов.

## Проверка

Вывод: позиции после шага, суммы по `x`/`y`, здоровье после урона; код возврата
`0` при ожидаемых значениях, иначе `1`.

## Решение

```cpp
#include <concepts>
#include <cstdint>
#include <iostream>
#include <print>
#include <tuple>
#include <type_traits>
#include <vector>

namespace {

using Entity = std::uint32_t;

template <class T>
concept ComponentType = std::is_trivially_copyable_v<T> &&
                        std::is_default_constructible_v<T>;

template <class C>
struct Column {
    std::vector<C> values;
    std::vector<char> present;
};

template <ComponentType... Cs>
class Registry {
public:
    Entity create() {
        const Entity entity = static_cast<Entity>(count_++);
        alive_.push_back(1);
        std::apply([&](auto&... columns) {
            (columns.values.emplace_back(), ...);
            (columns.present.push_back(0), ...);
        }, columns_);
        return entity;
    }

    template <ComponentType C>
    void add(Entity entity, const C& value) {
        auto& column = std::get<Column<C>>(columns_);
        column.values[entity] = value;
        column.present[entity] = 1;
    }

    template <ComponentType C>
    bool has(Entity entity) const {
        return std::get<Column<C>>(columns_).present[entity] != 0;
    }

    template <ComponentType C>
    C& get(Entity entity) {
        return std::get<Column<C>>(columns_).values[entity];
    }

    template <ComponentType... Target, class Func>
    void each(Func&& func) {
        for (Entity entity = 0; entity < count_; ++entity) {
            if (alive_[entity] != 0 && (has<Target>(entity) && ...)) {
                func(entity, get<Target>(entity)...);
            }
        }
    }

    Entity count() const { return count_; }

private:
    std::vector<char> alive_;
    std::tuple<Column<Cs>...> columns_;
    Entity count_ = 0;
};

struct Position { float x; float y; };
struct Velocity { float vx; float vy; };
struct Health { int hp; };

}  // namespace

int main() {
    Registry<Position, Velocity, Health> registry;

    const Entity a = registry.create();
    registry.add(a, Position{0.0f, 0.0f});
    registry.add(a, Velocity{1.0f, 0.5f});

    const Entity b = registry.create();
    registry.add(b, Position{10.0f, 10.0f});
    registry.add(b, Velocity{-1.0f, 0.0f});

    const Entity c = registry.create();          // tol'ko Position
    registry.add(c, Position{5.0f, 5.0f});

    const Entity d = registry.create();          // tol'ko Health
    registry.add(d, Health{42});

    constexpr float kDt = 2.0f;
    registry.each<Position, Velocity>([&](Entity, Position& position, Velocity& velocity) {
        position.x += velocity.vx * kDt;
        position.y += velocity.vy * kDt;
    });

    float sum_x = 0.0f;
    float sum_y = 0.0f;
    registry.each<Position>([&](Entity, Position& position) {
        sum_x += position.x;
        sum_y += position.y;
    });

    registry.each<Health>([&](Entity, Health& health) {
        health.hp -= 2;
    });

    std::println("a = ({}, {})", registry.get<Position>(a).x, registry.get<Position>(a).y);
    std::println("b = ({}, {})", registry.get<Position>(b).x, registry.get<Position>(b).y);
    std::println("c = ({}, {}) (not moved)", registry.get<Position>(c).x, registry.get<Position>(c).y);
    std::println("sum = ({}, {})", sum_x, sum_y);
    std::println("d.hp = {}", registry.get<Health>(d).hp);

    const bool ok = registry.get<Position>(a).x == 2.0f &&
                    registry.get<Position>(b).x == 8.0f &&
                    registry.get<Position>(c).x == 5.0f &&
                    sum_x == 15.0f && sum_y == 16.0f &&
                    registry.get<Health>(d).hp == 40;
    std::cout << "ecs done\n";
    return ok ? 0 : 1;
}
```
