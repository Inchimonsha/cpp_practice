// m02_ownership/ex06_copy_elision — kopiruyushchee opuskanie (RVO/NRVO).
// Standart: C++23. Zapusk: ./run.ps1 m02_ownership/ex06_copy_elision
#include <iostream>
#include <print>
#include <utility>

namespace {

struct Counted {
    static inline int ctors = 0;
    static inline int copies = 0;
    static inline int moves = 0;

    int value;

    explicit Counted(int v) : value(v) { ++ctors; }
    Counted(const Counted& other) : value(other.value) { ++copies; }
    Counted(Counted&& other) noexcept : value(other.value) { ++moves; }

    static void reset() { ctors = copies = moves = 0; }
};

// RVO: garantirovanno bez kopiy/pereemeshcheniy (C++17).
Counted make_prvalue() {
    return Counted(1);
}

// NRVO: obychno bez kopiy, no ne garantirovano.
Counted make_named() {
    Counted local(2);
    return local;
}

// std::move zapreshchaet NRVO -> odno peremeshchenie.
#if defined(__GNUC__) || defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpessimizing-move"
#endif
Counted make_move() {
    Counted local(3);
    return std::move(local);
}
#if defined(__GNUC__) || defined(__clang__)
#  pragma GCC diagnostic pop
#endif

void report(const char* label) {
    std::println("  {:<11} ctors={} copies={} moves={}",
                 label, Counted::ctors, Counted::copies, Counted::moves);
}

}  // namespace

int main() {
    Counted::reset();
    Counted a = make_prvalue();
    report("prvalue");
    const bool prvalue_elided = (Counted::copies == 0 && Counted::moves == 0);

    Counted::reset();
    Counted b = make_named();
    report("named");
    const bool named_elided = (Counted::copies == 0);

    Counted::reset();
    Counted c = make_move();
    report("std::move");
    const bool move_happened = (Counted::moves >= 1);

    std::println("values: a={} b={} c={}", a.value, b.value, c.value);
    std::cout << "copy elision done\n";
    return (prvalue_elided && named_elided && move_happened) ? 0 : 1;
}
