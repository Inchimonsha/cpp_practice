#pragma once

// Векторная математика для 2D-игр. Используется во всём курсе (th19 и далее),
// поэтому вынесена в общий модуль, а не в отдельные темы.
//
// C++20, без внешних зависимостей.

#include <cmath>

namespace cpp_course::math {

// 2D-вектор с координатами типа float.
//
// Дополнительно к sf::Vector2f (SFML) этот тип:
//   * не зависит от SFML (можно тестировать без графики);
//   * предоставляет математические операции из коробки.
// При переносе кода на SFML используйте sf::Vector2f или конвертацию
// (см. th19_math_coordinates_vectors, th23_sfml_window_loop).
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}

    // Арифметика (+=, -=) и умножение на скаляр.
    Vec2& operator+=(const Vec2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Vec2& operator-=(const Vec2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Vec2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
};

// Сравнение с допуском; точное == для float почти всегда ловушка.
inline bool near_equal(const Vec2& a, const Vec2& b, float eps = 1e-5f) {
    return std::abs(a.x - b.x) <= eps && std::abs(a.y - b.y) <= eps;
}

inline Vec2 operator+(Vec2 a, const Vec2& b) { return a += b; }
inline Vec2 operator-(Vec2 a, const Vec2& b) { return a -= b; }
inline Vec2 operator*(Vec2 v, float scalar)  { return v *= scalar; }
inline Vec2 operator*(float scalar, Vec2 v)  { return v *= scalar; }
inline Vec2 operator/(Vec2 v, float scalar)  { return v /= scalar; }
inline Vec2 operator-(const Vec2& v)         { return Vec2{-v.x, -v.y}; }

inline bool operator==(const Vec2& a, const Vec2& b) {
    return a.x == b.x && a.y == b.y;
}

// Квадрат длины — быстрее length(), отсутствует извлечение корня.
inline float length_squared(const Vec2& v) { return v.x * v.x + v.y * v.y; }

// Длина вектора. Не constexpr в C++20 (sqrt constexpr только с C++26).
inline float length(const Vec2& v) { return std::sqrt(length_squared(v)); }

// Скалярное произведение (dot product).
inline float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }

// Расстояние между точками.
inline float distance(const Vec2& a, const Vec2& b) { return length(a - b); }

// Нормализация. Для нулевого вектора возвращает нулевой вектор: вычисляем
// длину, проверяем её до деления (деление на ноль даёт inf/NaN).
inline Vec2 normalized(const Vec2& v) {
    const float len = length(v);
    return len > 0.0f ? Vec2{v.x / len, v.y / len} : Vec2{};
}

// Линейная интерполяция между точками (t из [0,1]).
inline Vec2 lerp(const Vec2& a, const Vec2& b, float t) {
    return Vec2{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

}  // namespace cpp_course::math