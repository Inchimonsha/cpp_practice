#include <print>
import math;

int main() {
    std::println("3+4={}", math::add(3, 4));
    std::println("3*4={}", math::mul(3, 4));
    return (math::add(3, 4) == 7 && math::mul(3, 4) == 12) ? 0 : 1;
}
