#include "shape.hpp"

namespace shapes {
double area_circle(double radius) {
    constexpr double kPi = 3.14159265358979323846;
    return kPi * radius * radius;
}

double area_rect(double width, double height) {
    return width * height;
}
}  // namespace shapes
