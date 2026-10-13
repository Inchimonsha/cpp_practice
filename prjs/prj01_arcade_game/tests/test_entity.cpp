// Тесты: AABB-коллизии и движение сущностей (без окна).

#include <arcade/entity.hpp>
#include <arcade/falling_object.hpp>
#include <arcade/player.hpp>

#include <cpp_course/math/vec2.hpp>
#include <cpp_course/test.hpp>

#include <cmath>

using arcade::Aabb;
using arcade::FallingObject;
using arcade::ObjectKind;
using arcade::Player;
using cpp_course::math::Vec2;

namespace {
// Сравнение Aabb по полям с допуском (near_equal работает с Vec2).
void check_aabb(const Aabb& a, const Aabb& b, float eps = 1e-5f) {
    CHECK_NEAR(a.x, b.x, eps);
    CHECK_NEAR(a.y, b.y, eps);
    CHECK_NEAR(a.w, b.w, eps);
    CHECK_NEAR(a.h, b.h, eps);
}
}  // namespace

CPP_COURSE_TEST("Aabb: непересекающиеся") {
    const Aabb a{0.0f, 0.0f, 10.0f, 10.0f};
    const Aabb b{20.0f, 0.0f, 10.0f, 10.0f};
    CHECK(!a.intersects(b));
    CHECK(!b.intersects(a));
}

CPP_COURSE_TEST("Aabb: пересечение в углах и по центру") {
    const Aabb a{0.0f, 0.0f, 10.0f, 10.0f};
    const Aabb corner{8.0f, 8.0f, 10.0f, 10.0f};
    const Aabb center{4.0f, 4.0f, 2.0f, 2.0f};
    CHECK(a.intersects(corner));
    CHECK(corner.intersects(a));
    CHECK(a.intersects(center));
}

CPP_COURSE_TEST("Aabb: касание гранями — пересечение") {
    const Aabb a{0.0f, 0.0f, 10.0f, 10.0f};
    const Aabb touch{10.0f, 0.0f, 10.0f, 10.0f};
    CHECK(a.intersects(touch));
}

CPP_COURSE_TEST("Aabb: вложенность") {
    const Aabb outer{0.0f, 0.0f, 40.0f, 40.0f};
    const Aabb inner{10.0f, 10.0f, 5.0f, 5.0f};
    CHECK(outer.intersects(inner));
    CHECK(inner.intersects(outer));
}

CPP_COURSE_TEST("Entity: движение и границы") {
    arcade::Entity e{Aabb{100.0f, 100.0f, 20.0f, 20.0f}};
    e.move(Vec2{10.0f, -5.0f});
    check_aabb(e.bounds(), Aabb{110.0f, 95.0f, 20.0f, 20.0f});
}

CPP_COURSE_TEST("Entity: clamp_x удерживает внутри поля") {
    arcade::Entity e{Aabb{-5.0f, 0.0f, 20.0f, 20.0f}};
    e.clamp_x(0.0f, 100.0f);
    check_aabb(e.bounds(), Aabb{0.0f, 0.0f, 20.0f, 20.0f});

    e.set_bounds(Aabb{95.0f, 0.0f, 20.0f, 20.0f});
    e.clamp_x(0.0f, 100.0f);
    check_aabb(e.bounds(), Aabb{80.0f, 0.0f, 20.0f, 20.0f});
}

CPP_COURSE_TEST("Player: движение с dt не зависит от FPS") {
    Player p{Aabb{0.0f, 0.0f, 60.0f, 20.0f}, 240.0f};

    // 60 кадров по 1/60 c = 1 секунда движения вправо.
    for (int i = 0; i < 60; ++i) {
        p.move_right(1.0f / 60.0f);
    }
    // 30 кадров по 1/30 c = также 1 секунда.
    Player q{Aabb{0.0f, 0.0f, 60.0f, 20.0f}, 240.0f};
    for (int i = 0; i < 30; ++i) {
        q.move_right(1.0f / 30.0f);
    }

    CHECK(std::abs(p.bounds().x - q.bounds().x) < 0.001f);
}

CPP_COURSE_TEST("Player: clamp не даёт выйти за поле") {
    Player p{Aabb{0.0f, 0.0f, 60.0f, 20.0f}, 500.0f};
    p.move_left(1.0f);
    p.clamp_to_field(0.0f, 800.0f);
    CHECK_EQ(p.bounds().x, 0.0f);

    p.move_right(10.0f);
    p.clamp_to_field(0.0f, 800.0f);
    CHECK_EQ(p.bounds().x, 740.0f);  // 800 - 60
}

CPP_COURSE_TEST("FallingObject: падает вниз с dt") {
    FallingObject obj{Aabb{0.0f, 0.0f, 30.0f, 30.0f}, ObjectKind::good, 100.0f};
    obj.update(1.0f);
    check_aabb(obj.bounds(), Aabb{0.0f, 100.0f, 30.0f, 30.0f});
}

CPP_COURSE_TEST("FallingObject: столкновение через bounds") {
    FallingObject obj{Aabb{100.0f, 100.0f, 30.0f, 30.0f}, ObjectKind::bad, 0.0f};
    Player p{Aabb{110.0f, 110.0f, 60.0f, 20.0f}};
    CHECK(p.bounds().intersects(obj.bounds()));
}