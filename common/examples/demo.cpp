// Демонстрация возможностей общего модуля.
// Сборка: cpp_course_demo  (см. common/CMakeLists.txt).

#include <cpp_course/log.hpp>
#include <cpp_course/math/vec2.hpp>
#include <cpp_course/test.hpp>
#include <cpp_course/version.hpp>

#include <iostream>

using cpp_course::math::Vec2;

int main() {
    // Логгер
    cpp_course::log().set_prefix("demo");
    cpp_course::log().info("старт демо");

    // Версия
    std::cout << "cpp_course v" << cpp_course::version_string << '\n';

    // Векторы
    const Vec2 position{100.0f, 200.0f};
    const Vec2 velocity{3.0f, -1.0f};
    const Vec2 movement = velocity * 0.5f;

    std::cout << "position      : " << position.x << ", " << position.y << '\n';
    std::cout << "move          : " << movement.x << ", " << movement.y << '\n';
    std::cout << "length(v)     : " << cpp_course::math::length(velocity) << '\n';
    std::cout << "normalized(3,4): "
              << cpp_course::math::normalized(Vec2{3.0f, 4.0f}).x << ", "
              << cpp_course::math::normalized(Vec2{3.0f, 4.0f}).y << '\n';
    std::cout << "dot((1,0),(0,1)) : " << cpp_course::math::dot(Vec2{1.0f, 0.0f}, Vec2{0.0f, 1.0f}) << '\n';

    cpp_course::log().info("демо завершено");
    return 0;
}