#include <iomanip>
#include <iostream>

#include "shape.hpp"

int main() {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "circle r=1: " << shapes::area_circle(1.0) << "\n";
    std::cout << "rect 2x3: " << shapes::area_rect(2.0, 3.0) << "\n";
    return shapes::area_rect(2.0, 3.0) == 6.0 ? 0 : 1;
}
