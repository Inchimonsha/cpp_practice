// m05_errors/ex02_noexcept — noexcept i rost kontejnerov.
// Standart: C++23. Zapusk: ./run.ps1 m05_errors/ex02_noexcept
#include <iostream>
#include <print>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

struct Counters {
    static inline int copies = 0;
    static inline int moves = 0;
    static void reset() { copies = moves = 0; }
};

// Peremeshchenie noexcept -> vector peremeshchaet.
struct NoThrowMove {
    int value{};
    explicit NoThrowMove(int v) : value(v) {}
    NoThrowMove(const NoThrowMove& other) : value(other.value) { ++Counters::copies; }
    NoThrowMove(NoThrowMove&& other) noexcept : value(other.value) { ++Counters::moves; }
};

// Peremeshchenie mozhet brosat' -> vector kopiruet pri roste.
struct ThrowingMove {
    int value{};
    explicit ThrowingMove(int v) : value(v) {}
    ThrowingMove(const ThrowingMove& other) : value(other.value) { ++Counters::copies; }
    ThrowingMove(ThrowingMove&& other) : value(other.value) { ++Counters::moves; }
};

}  // namespace

int main() {
    static_assert(std::is_nothrow_move_constructible_v<NoThrowMove>);
    static_assert(!std::is_nothrow_move_constructible_v<ThrowingMove>);

    Counters::reset();
    {
        std::vector<NoThrowMove> v;
        v.reserve(1);
        for (int i = 0; i < 12; ++i) { v.push_back(NoThrowMove{i}); }
    }
    std::println("NoThrowMove:  copies={} moves={}", Counters::copies, Counters::moves);
    const bool no_throw_used_moves = (Counters::copies == 0 && Counters::moves > 0);

    Counters::reset();
    {
        std::vector<ThrowingMove> v;
        v.reserve(1);
        for (int i = 0; i < 12; ++i) { v.push_back(ThrowingMove{i}); }
    }
    std::println("ThrowingMove: copies={} moves={}", Counters::copies, Counters::moves);
    const bool throwing_used_copies = (Counters::copies > 0);

    std::println("trait NoThrowMove   nothrow_move = {}",
                 std::is_nothrow_move_constructible_v<NoThrowMove>);
    std::println("trait ThrowingMove  nothrow_move = {}",
                 std::is_nothrow_move_constructible_v<ThrowingMove>);

    const bool expr_ok = noexcept(std::declval<std::vector<int>&>().push_back(1));
    std::println("noexcept(vector<int>::push_back) = {}", expr_ok);

    std::cout << "noexcept done\n";
    return (no_throw_used_moves && throwing_used_copies) ? 0 : 1;
}
