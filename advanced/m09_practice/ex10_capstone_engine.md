# ex10_capstone_engine — мини-движок (ECS + события + FSM + CMake)

## Теория

Итоговый проект связывает приёмы курса в один небольшой, но настоящий движок:

- **ECS** (`Registry<Position, Velocity, Health>`) хранит сущности «по столбцам» и
  выполняет системы через `each<...>`.
- **Шина событий** (`EventBus`) разрывает связь между «нанесением урона» и
  изменением здоровья: система урона подписана на `Damaged`.
- **Состояния игры** (`Phase`) управляют тем, какие системы работают: вне
  `Playing` мир замирает.
- **Сборка CMake**: статическая библиотека `engine`, демо `engine_demo`, тесты
  `engine_tests`, зарегистрированные в CTest.

Разделение ответственности: заголовки — интерфейсы, `src/game.cpp` — системы и
правила, `src/main.cpp` — сценарий, `tests/` — проверки.

## Синтаксис

```cpp
Registry<Position, Velocity, Health> world_;
world_.create();
world_.add(entity, Position{{0.0, 0.0}});
world_.each<Position, Velocity>([dt](Entity, Position& p, Velocity& v) {
    p.value += v.value * dt;
});
world_.destroy(entity);

events_.subscribe<Damaged>([this](const Damaged& event) { /* ... */ });
events_.publish(Damaged{entity, 10});
```

```cmake
add_library(engine src/game.cpp)
target_include_directories(engine PUBLIC include)
add_executable(engine_demo src/main.cpp)
target_link_libraries(engine_demo PRIVATE engine)
enable_testing()
add_test(NAME engine_tests COMMAND engine_tests)
```

## Подводные камни

- Порядок систем важен: движение до проверки смертей; иначе сущность умрёт «на
  старом» месте.
- Удаление — флаг `alive`, а не стирание из массивов: индексы (entity id) должны
  оставаться стабильными.
- Инициализация `EventBus` лямбдой, захватывающей `this`, ведёт к самоссылке —
  поэтому реестр и шина объявлены рядом, а подписка идёт в конструкторе при
  полностью сконструированных полях.
- Заголовки подключаются с префиксом `engine/`; каталог `include/` должен быть
  `PUBLIC` у цели-библиотеки.
- `std::println` требует `-lstdc++exp` на GCC; в CMake это
  `target_link_libraries(... PRIVATE stdc++exp)`.
- Для multi-file проекта запускайте тесты через `ctest`, а не вручную.

## Задача

Собрать мини-движок: ECS-реестр (создание, компоненты, системы, удаление),
типобезопасная шина событий, конечный автомат состояний игры (`Menu → Playing →
GameOver`), системы движения и урона, и CMake-проект с демо и тестами. Demo
прогоняет сценарий и проверяет позиции, число живых врагов и переход в
`GameOver`.

## Где применяется

- Каркас 2D-игры/песочницы, интеграция подсистем, шаблон для реальных проектов
  курса (SFML + Lua добавляются поверх этого каркаса).

## Проверка

```bash
cmake -S . -B build -G "MinGW Makefiles" ^
  -DCMAKE_MAKE_PROGRAM=C:/msys64/mingw64/bin/mingw32-make.exe ^
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
cmake --build build
ctest --test-dir build --output-on-failure
build\engine_demo.exe
```

Демо печатает позиции и фазу и завершается кодом `0`; `ctest` сообщает
`100% tests passed`. Быстрый путь: `build.cmd`.

## Решение

Структура `ex10_capstone_engine/`:

```
CMakeLists.txt
build.cmd
include/engine/vec2.hpp
include/engine/ecs.hpp
include/engine/event_bus.hpp
include/engine/game.hpp
src/game.cpp
src/main.cpp
tests/test_engine.cpp
```

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(capstone_engine CXX)

add_library(engine src/game.cpp)
target_include_directories(engine PUBLIC include)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

add_executable(engine_demo src/main.cpp)
target_link_libraries(engine_demo PRIVATE engine stdc++exp)
target_compile_options(engine_demo PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

enable_testing()
add_executable(engine_tests tests/test_engine.cpp)
target_link_libraries(engine_tests PRIVATE engine stdc++exp)
target_compile_options(engine_tests PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
add_test(NAME engine_tests COMMAND engine_tests)
```

`include/engine/vec2.hpp`:

```cpp
#pragma once

namespace engine {

struct Vec2 {
    double x;
    double y;
};

inline Vec2 operator+(const Vec2& a, const Vec2& b) { return {a.x + b.x, a.y + b.y}; }
inline Vec2 operator-(const Vec2& a, const Vec2& b) { return {a.x - b.x, a.y - b.y}; }
inline Vec2 operator*(const Vec2& v, double scalar) { return {v.x * scalar, v.y * scalar}; }

inline Vec2& operator+=(Vec2& a, const Vec2& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline bool operator==(const Vec2& a, const Vec2& b) {
    return a.x == b.x && a.y == b.y;
}

}  // namespace engine
```

`include/engine/ecs.hpp`:

```cpp
#pragma once

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <vector>

namespace engine {

using Entity = std::uint32_t;

template <class T>
concept ComponentType = std::is_trivially_copyable_v<T> &&
                        std::is_default_constructible_v<T>;

template <ComponentType... Cs>
class Registry {
    template <class C>
    struct Column {
        std::vector<C> values;
        std::vector<char> present;
    };

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

    template <ComponentType C>
    const C& get(Entity entity) const {
        return std::get<Column<C>>(columns_).values[entity];
    }

    bool alive(Entity entity) const {
        return entity < count_ && alive_[entity] != 0;
    }

    void destroy(Entity entity) {
        if (alive(entity)) { alive_[entity] = 0; }
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

}  // namespace engine
```

`include/engine/event_bus.hpp`:

```cpp
#pragma once

#include <any>
#include <cstdint>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace engine {

class EventBus {
public:
    using Token = std::uint64_t;

    template <class E, class F>
    Token subscribe(F&& handler) {
        const Token token = next_token_++;
        handlers_[std::type_index(typeid(E))].push_back(
            {token, [fn = std::forward<F>(handler)](const std::any& payload) {
                 fn(std::any_cast<const E&>(payload));
             }});
        return token;
    }

    template <class E>
    void unsubscribe(Token token) {
        auto it = handlers_.find(std::type_index(typeid(E)));
        if (it == handlers_.end()) { return; }
        auto& list = it->second;
        for (auto slot = list.begin(); slot != list.end(); ++slot) {
            if (slot->id == token) { list.erase(slot); return; }
        }
    }

    template <class E>
    void publish(const E& event) const {
        auto it = handlers_.find(std::type_index(typeid(E)));
        if (it == handlers_.end()) { return; }
        for (const auto& slot : it->second) {
            slot.handler(std::any{event});
        }
    }

private:
    struct Entry {
        Token id;
        std::function<void(const std::any&)> handler;
    };

    std::unordered_map<std::type_index, std::vector<Entry>> handlers_;
    Token next_token_ = 1;
};

}  // namespace engine
```

`include/engine/game.hpp`:

```cpp
#pragma once

#include "engine/ecs.hpp"
#include "engine/event_bus.hpp"
#include "engine/vec2.hpp"

#include <vector>

namespace engine {

struct Position { Vec2 value; };
struct Velocity { Vec2 value; };
struct Health { int hp; };

enum class Phase { Menu, Playing, GameOver };

struct Damaged {
    Entity target;
    int amount;
};

class Game {
public:
    Game();

    Entity spawn_player(Vec2 position);
    Entity spawn_enemy(Vec2 position);

    void start();
    void tick(double dt);
    void damage(Entity target, int amount);

    Phase phase() const { return phase_; }
    Entity player() const { return player_; }
    int living_enemies() const;

    Vec2 position_of(Entity entity) const { return world_.get<Position>(entity).value; }
    int hp_of(Entity entity) const { return world_.get<Health>(entity).hp; }

private:
    void movement_system(double dt);

    Registry<Position, Velocity, Health> world_;
    EventBus events_;
    std::vector<Entity> enemies_;
    Entity player_ = 0;
    Phase phase_ = Phase::Menu;
};

}  // namespace engine
```

`src/game.cpp`:

```cpp
#include "engine/game.hpp"

namespace engine {

Game::Game() {
    events_.subscribe<Damaged>([this](const Damaged& event) {
        if (world_.alive(event.target) && world_.has<Health>(event.target)) {
            world_.get<Health>(event.target).hp -= event.amount;
        }
    });
}

Entity Game::spawn_player(Vec2 position) {
    const Entity entity = world_.create();
    world_.add(entity, Position{position});
    world_.add(entity, Velocity{{1.0, 0.0}});
    world_.add(entity, Health{100});
    player_ = entity;
    return entity;
}

Entity Game::spawn_enemy(Vec2 position) {
    const Entity entity = world_.create();
    world_.add(entity, Position{position});
    world_.add(entity, Velocity{{-1.0, 0.0}});
    world_.add(entity, Health{5});
    enemies_.push_back(entity);
    return entity;
}

void Game::start() {
    if (phase_ == Phase::Menu) { phase_ = Phase::Playing; }
}

void Game::damage(Entity target, int amount) {
    events_.publish(Damaged{target, amount});
}

void Game::movement_system(double dt) {
    world_.each<Position, Velocity>([dt](Entity, Position& position, Velocity& velocity) {
        position.value += velocity.value * dt;
    });
}

void Game::tick(double dt) {
    if (phase_ != Phase::Playing) { return; }

    movement_system(dt);

    for (const Entity enemy : enemies_) {
        if (world_.alive(enemy) && world_.get<Health>(enemy).hp <= 0) {
            world_.destroy(enemy);
        }
    }

    if (world_.alive(player_) && world_.get<Health>(player_).hp <= 0) {
        phase_ = Phase::GameOver;
    }
}

int Game::living_enemies() const {
    int count = 0;
    for (const Entity enemy : enemies_) {
        if (world_.alive(enemy) && world_.get<Health>(enemy).hp > 0) { ++count; }
    }
    return count;
}

}  // namespace engine
```

`src/main.cpp`:

```cpp
#include "engine/game.hpp"

#include <iostream>
#include <print>

using namespace engine;

namespace {

const char* phase_name(Phase phase) {
    switch (phase) {
        case Phase::Menu: return "Menu";
        case Phase::Playing: return "Playing";
        case Phase::GameOver: return "GameOver";
    }
    return "?";
}

}  // namespace

int main() {
    Game game;
    const Entity player = game.spawn_player({0.0, 0.0});
    const Entity enemy_a = game.spawn_enemy({5.0, 0.0});
    const Entity enemy_b = game.spawn_enemy({-4.0, 4.0});

    std::println("phase = {}", phase_name(game.phase()));

    game.start();
    game.tick(1.0);

    std::println("player = ({}, {})", game.position_of(player).x, game.position_of(player).y);
    std::println("enemy_a = ({}, {})", game.position_of(enemy_a).x, game.position_of(enemy_a).y);
    std::println("enemy_b = ({}, {})", game.position_of(enemy_b).x, game.position_of(enemy_b).y);
    std::println("living enemies = {}", game.living_enemies());

    game.damage(enemy_a, 5);
    game.tick(0.5);

    const double player_x = game.position_of(player).x;
    const double enemy_a_x = game.position_of(enemy_a).x;
    const int living = game.living_enemies();

    game.damage(player, 100);
    game.tick(0.1);

    std::println("after hit: player_x = {}, enemy_a_x = {}, living = {}",
                 player_x, enemy_a_x, living);
    std::println("phase after player death = {}", phase_name(game.phase()));

    const bool ok = player_x == 1.5 && enemy_a_x == 3.5 && living == 1 &&
                    game.phase() == Phase::GameOver;
    std::cout << "capstone done\n";
    return ok ? 0 : 1;
}
```

`tests/test_engine.cpp`:

```cpp
#include "engine/game.hpp"

#include <iostream>
#include <print>

using namespace engine;

namespace {

int failures = 0;

#define CHECK(condition)                                                   \
    do {                                                                   \
        if (!(condition)) {                                                \
            std::println("FAIL {}:{} {}", __FILE__, __LINE__, #condition);  \
            ++failures;                                                    \
        }                                                                  \
    } while (false)

}  // namespace

int main() {
    Game game;
    const Entity player = game.spawn_player({0.0, 0.0});
    const Entity enemy = game.spawn_enemy({5.0, 0.0});

    CHECK(game.phase() == Phase::Menu);
    game.tick(1.0);                       // v Menu sistemy ne rabotayut
    CHECK(game.position_of(player) == (Vec2{0.0, 0.0}));

    game.start();
    CHECK(game.phase() == Phase::Playing);
    game.tick(1.0);
    CHECK(game.position_of(player) == (Vec2{1.0, 0.0}));
    CHECK(game.position_of(enemy) == (Vec2{4.0, 0.0}));
    CHECK(game.living_enemies() == 1);

    game.damage(enemy, 5);
    CHECK(game.hp_of(enemy) == 0);
    game.tick(0.1);
    CHECK(game.living_enemies() == 0);

    game.damage(player, 100);
    game.tick(0.1);
    CHECK(game.phase() == Phase::GameOver);
    game.tick(1.0);                       // posle GameOver mir zamiraet
    CHECK(game.phase() == Phase::GameOver);

    if (failures == 0) {
        std::println("all engine tests passed");
        return 0;
    }
    std::println("{} failure(s)", failures);
    return 1;
}
```
