// m13_interop_ffi/ex01_c_abi — C-ABI: extern "C", nepryamoy vyyzov, neprozrachnyy handle.
// Standart: C++23. Zapusk: ./run.ps1 m13_interop_ffi/ex01_c_abi
#include <cstdint>
#include <iostream>
#include <print>
#include <type_traits>

extern "C" {

struct Point {
    double x;
    double y;
};

int cpp_add(int left, int right) {
    return left + right;
}

double point_length_squared(Point point) {
    return point.x * point.x + point.y * point.y;
}
}

struct Counter {
    int value;
};

extern "C" Counter* counter_create(int start) {
    return new Counter{start};
}

extern "C" void counter_add(Counter* counter, int delta) {
    counter->value += delta;
}

extern "C" int counter_value(const Counter* counter) {
    return counter->value;
}

extern "C" void counter_destroy(Counter* counter) {
    delete counter;
}

int main() {
    static_assert(std::is_standard_layout_v<Point>, "Point dolzhen byt' standard-layout");
    static_assert(std::is_trivially_copyable_v<Point>, "Point dolzhen byt' trivially copyable");

    using add_function = int (*)(int, int);
    const add_function add = &cpp_add;
    const int sum = add(3, 4);
    std::println("add(3, 4) = {}", sum);

    const double length_squared = point_length_squared(Point{3.0, 4.0});
    std::println("point len^2 = {}", length_squared);

    Counter* counter = counter_create(0);
    counter_add(counter, 3);
    counter_add(counter, 7);
    const int value = counter_value(counter);
    counter_destroy(counter);
    std::println("counter value = {}", value);

    const bool ok = sum == 7 && length_squared == 25.0 && value == 10;
    std::cout << "c abi done\n";
    return ok ? 0 : 1;
}
