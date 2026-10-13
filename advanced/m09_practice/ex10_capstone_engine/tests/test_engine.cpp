#include "engine/game.hpp"

#include <iostream>
#include <print>

using namespace engine;

namespace {

int failures = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            std::println("FAIL {}:{} {}", __FILE__, __LINE__, #condition);      \
            ++failures;                                                         \
        }                                                                       \
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
