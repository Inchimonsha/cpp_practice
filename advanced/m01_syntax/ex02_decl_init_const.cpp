// m01_syntax/ex02_decl_init_const — obyavlenie, inicializaciya, const.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex02_decl_init_const
#include <iostream>
#include <print>
#include <vector>

namespace {

constexpr int square(int x) { return x * x; }   // mozhet byt' i runtime
consteval int cube(int x) { return x * x * x; } // tol'ko compile-time

struct Point {
    int x;
    int y;
};

int next_id() {
    static int id = 0;  // zhivet mezhdu vyzovami
    return ++id;
}

}  // namespace

constinit inline int g_seed = 12345;  // static init, bez poryadkovyh sluchajnostej

int main() {
    static_assert(square(5) == 25);
    static_assert(cube(3) == 27);  // consteval korrektno v static_assert

    const int a{};
    const int b{7};
    const int c(7);
    const int d = 7;
    const std::vector<int> list{1, 2, 3};

    std::println("a={} b={} c={} d={} list.size={}", a, b, c, d, list.size());

    // Odin element so znacheniem 5 vs pyat' nulej — raznica ochen' vazhna.
    const std::vector<int> one{5};
    const std::vector<int> five(5);
    std::println("one.size={} five.size={}", one.size(), five.size());

    const Point p{.x = 1, .y = 2};
    std::println("Point({}, {})", p.x, p.y);

    const int id1 = next_id();
    const int id2 = next_id();
    const int id3 = next_id();
    std::println("next_id={} {} {}", id1, id2, id3);
    std::println("square(9)={} cube(2)={} g_seed={}", square(9), cube(2), g_seed);

    std::cout << "decl/init done\n";
    return 0;
}
