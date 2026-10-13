#pragma once

// Падающий предмет. Модель без SFML.

#include <arcade/entity.hpp>

namespace arcade {

enum class ObjectKind {
    good,  // ловля даёт очки
    bad,   // ловля отнимает жизнь
};

class FallingObject {
public:
    FallingObject(Aabb bounds, ObjectKind kind, float fall_speed)
        : entity_(bounds), kind_(kind), fall_speed_(fall_speed) {}

    ObjectKind kind() const { return kind_; }

    float fall_speed() const { return fall_speed_; }

    // Движение вниз с учётом dt (пиксели/секунду).
    void update(float dt) { entity_.move({0.0f, fall_speed_ * dt}); }

    const Aabb& bounds() const { return entity_.bounds(); }

private:
    Entity entity_;
    ObjectKind kind_;
    float fall_speed_;
};

}  // namespace arcade