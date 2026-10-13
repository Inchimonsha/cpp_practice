#pragma once

// AABB-прямоугольник и базовая сущность с позицией/размером.
// Чистая логика — без SFML, тестируется без окна.

#include <cpp_course/math/vec2.hpp>

namespace arcade {

// Axis-Aligned Bounding Box: прямоугольник, стороны которого выровнены по
// осям. x, y — верхний левый угол; w, h — ширина и высота.
struct Aabb {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    // Пересечение двух прямоугольников. Касание гранями засчитывается как
    // пересечение (включительные <=). Для «простой аркады — ловля по касанию».
    // Платформерам обычно нужно строгое пересечение (<): разбор в th22.
    bool intersects(const Aabb& other) const {
        return x <= other.x + other.w && other.x <= x + w &&
               y <= other.y + other.h && other.y <= y + h;
    }

    // Центр прямоугольника (удобно для рендера и размещения).
    cpp_course::math::Vec2 center() const {
        return {x + w / 2.0f, y + h / 2.0f};
    }
};

// Базовая сущность: неподвижный/подвижный объект с гранителями.
class Entity {
public:
    Entity() = default;
    explicit Entity(Aabb bounds) : bounds_(bounds) {}

    const Aabb& bounds() const { return bounds_; }
    void set_bounds(Aabb bounds) { bounds_ = bounds; }

    // Плавное перемещение на delta (в пикселях).
    void move(cpp_course::math::Vec2 delta) {
        bounds_.x += delta.x;
        bounds_.y += delta.y;
    }

    // Ограничение координат: не даёт сущности выйти за [min_x, max_x].
    void clamp_x(float min_x, float max_x);

    bool collides_with(const Entity& other) const {
        return bounds_.intersects(other.bounds_);
    }

private:
    Aabb bounds_;
};

}  // namespace arcade