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
