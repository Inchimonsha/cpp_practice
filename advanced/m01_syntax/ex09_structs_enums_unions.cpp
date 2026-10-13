// m01_syntax/ex09_structs_enums_unions — struct, enum, union (i variant).
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex09_structs_enums_unions
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace {

struct RenderConfig {
    int width{};
    int height{};
    bool fullscreen{};
};

enum class State { idle, run, jump };

[[nodiscard]] std::string_view to_string(State state) {
    switch (state) {
        case State::idle: return "idle";
        case State::run:  return "run";
        case State::jump: return "jump";
    }
    return "unknown";
}

// Bitfield: upakovka flagov v odno slovo.
struct CellFlags {
    unsigned visible : 1;
    unsigned solid   : 1;
    unsigned liquid  : 1;
    unsigned layer   : 4;
};

// variant vmesto union: bezopasno, znaem aktivnyj tip, est' std::visit.
using Value = std::variant<int, std::string, double>;

[[nodiscard]] std::string render(const Value& value) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            return "int:" + std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return "double:" + std::to_string(v);
        } else {
            return "string:" + v;
        }
    }, value);
}

}  // namespace

int main() {
    const RenderConfig config{.width = 1280, .height = 720, .fullscreen = false};
    std::println("config {}x{} fullscreen={}", config.width, config.height,
                 config.fullscreen);

    std::println("state = {}", to_string(State::run));

    const CellFlags cell{.visible = 1, .solid = 1, .liquid = 0, .layer = 5};
    const unsigned packed = (cell.visible) | (cell.solid << 1)
                          | (cell.liquid << 2) | (cell.layer << 3);
    std::println("packed cell flags = {}", packed);

    std::println("{}", render(Value{42}));
    std::println("{}", render(Value{std::string("hello")}));
    std::println("{}", render(Value{2.5}));

    std::cout << "structs/enums/variant done\n";
    return packed == 43 ? 0 : 1;
}
