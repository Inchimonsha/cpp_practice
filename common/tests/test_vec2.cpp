// Тесты векторной математики.

#include <cpp_course/math/vec2.hpp>
#include <cpp_course/test.hpp>

#include <cmath>

using cpp_course::math::Vec2;
using cpp_course::math::lerp;
using cpp_course::math::distance;
using cpp_course::math::length;
using cpp_course::math::length_squared;
using cpp_course::math::near_equal;
using cpp_course::math::normalized;
using cpp_course::math::dot;

CPP_COURSE_TEST("Vec2: конструктор и доступ") {
    const Vec2 v{3.0f, -4.0f};
    CHECK_EQ(v.x, 3.0f);
    CHECK_EQ(v.y, -4.0f);
    const Vec2 def;
    CHECK_EQ(def.x, 0.0f);
    CHECK_EQ(def.y, 0.0f);
}

CPP_COURSE_TEST("Vec2: арифметика") {
    CHECK((Vec2{1.0f, 2.0f} + Vec2{3.0f, 4.0f}) == Vec2{4.0f, 6.0f});
    CHECK((Vec2{3.0f, 2.0f} - Vec2{1.0f, 1.0f}) == Vec2{2.0f, 1.0f});
    CHECK((Vec2{2.0f, -3.0f} * 2.0f) == Vec2{4.0f, -6.0f});
    CHECK((Vec2{2.0f, 4.0f} / 2.0f) == Vec2{1.0f, 2.0f});
    CHECK((-Vec2{1.0f, 2.0f}) == Vec2{-1.0f, -2.0f});
}

CPP_COURSE_TEST("Vec2: += -= и *= ") {
    Vec2 v{1.0f, 1.0f};
    v += Vec2{2.0f, 3.0f};
    CHECK(v == Vec2{3.0f, 4.0f});
    v -= Vec2{1.0f, 1.0f};
    CHECK(v == Vec2{2.0f, 3.0f});
    v *= 2.0f;
    CHECK(v == Vec2{4.0f, 6.0f});
    v /= 2.0f;
    CHECK_EQ(v.x, 2.0f);
    CHECK_EQ(v.y, 3.0f);
}

CPP_COURSE_TEST("Vec2: длина и расстояние") {
    CHECK_NEAR(length(Vec2{3.0f, 4.0f}), 5.0f, 1e-6f);
    CHECK_NEAR(length_squared(Vec2{3.0f, 4.0f}), 25.0f, 1e-6f);
    CHECK_NEAR(distance(Vec2{0.0f, 0.0f}, Vec2{0.0f, 5.0f}), 5.0f, 1e-6f);
    CHECK_NEAR(distance(Vec2{1.0f, 1.0f}, Vec2{4.0f, 5.0f}), 5.0f, 1e-6f);
}

CPP_COURSE_TEST("Vec2: длина нулевого вектора") {
    CHECK_NEAR(length(Vec2{}), 0.0f, 1e-6f);
}

CPP_COURSE_TEST("Vec2: скалярное произведение") {
    CHECK_NEAR(dot(Vec2{1.0f, 0.0f}, Vec2{1.0f, 0.0f}), 1.0f, 1e-6f);
    CHECK_NEAR(dot(Vec2{1.0f, 0.0f}, Vec2{0.0f, 1.0f}), 0.0f, 1e-6f);
    CHECK_NEAR(dot(Vec2{3.0f, 4.0f}, Vec2{2.0f, -1.0f}), 2.0f, 1e-6f);
}

CPP_COURSE_TEST("Vec2: нормализация") {
    CHECK(near_equal(normalized(Vec2{3.0f, 4.0f}), Vec2{0.6f, 0.8f}, 1e-5f));
    CHECK_NEAR(length(normalized(Vec2{10.0f, 0.0f})), 1.0f, 1e-6f);
}

CPP_COURSE_TEST("Vec2: нормализация нулевого вектора не даёт NaN") {
    const Vec2 zero = normalized(Vec2{});
    CHECK(std::isfinite(zero.x));
    CHECK(std::isfinite(zero.y));
}

CPP_COURSE_TEST("Vec2: интерполяция") {
    CHECK(lerp(Vec2{0.0f, 0.0f}, Vec2{10.0f, 20.0f}, 0.5f) == Vec2{5.0f, 10.0f});
    CHECK(lerp(Vec2{0.0f, 0.0f}, Vec2{10.0f, 0.0f}, 0.0f) == Vec2{0.0f, 0.0f});
    CHECK(lerp(Vec2{0.0f, 0.0f}, Vec2{10.0f, 0.0f}, 1.0f) == Vec2{10.0f, 0.0f});
}