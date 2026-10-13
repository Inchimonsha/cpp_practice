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
