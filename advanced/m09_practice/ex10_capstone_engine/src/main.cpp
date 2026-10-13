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
    std::println("after hit: player_x = {}, enemy_a_x = {}, living = {}",
                 player_x, enemy_a_x, living);

    game.damage(player, 100);
    game.tick(0.1);
    std::println("phase after player death = {}", phase_name(game.phase()));

    const bool ok = player_x == 1.5 && enemy_a_x == 3.5 && living == 1 &&
                    game.phase() == Phase::GameOver;
    std::cout << "capstone done\n";
    return ok ? 0 : 1;
}
