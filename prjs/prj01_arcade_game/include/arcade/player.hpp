#pragma once

// Игрок (платформа). Модель без SFML: позиция/размер/движение по X.
// Скорость не зависит от FPS — движение всегда через update(dt).

#include <arcade/entity.hpp>

namespace arcade {

class Player {
public:
    // bounds — стартовые гранители; speed — пикселей в секунду.
    explicit Player(Aabb bounds, float speed = 240.0f);

    float speed() const { return speed_; }
    void set_speed(float speed) { speed_ = speed; }

    // Движение с учётом времени.
    void move_left(float dt);
    void move_right(float dt);

    // Ограничение позиции: платформа не выходит за поле [min_x, max_x].
    void clamp_to_field(float min_x, float max_x);

    const Aabb& bounds() const { return bounds_; }
    void set_bounds(Aabb bounds) { bounds_ = bounds; }

private:
    Aabb bounds_;
    float speed_;
};

}  // namespace arcade