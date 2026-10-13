// m02_ownership/ex05_perfect_forwarding — ideal'naya peredacha.
// Standart: C++23. Zapusk: ./run.ps1 m02_ownership/ex05_perfect_forwarding
#include <iostream>
#include <memory>
#include <print>
#include <string>
#include <type_traits>
#include <utility>

namespace {

const char* classify(int&) { return "lvalue"; }
const char* classify(int&&) { return "rvalue"; }

// Obyortka: sohranyaet kategoriyu argumenta.
template <class T>
void relay(T&& arg) {
    std::println("  relay sees: {}", classify(std::forward<T>(arg)));
}

// Tip posle svertyvaniya ssylok.
template <class T>
const char* ref_kind() {
    if constexpr (std::is_lvalue_reference_v<T>) {
        return "lvalue-reference";
    } else if constexpr (std::is_rvalue_reference_v<T>) {
        return "rvalue-reference";
    } else {
        return "value";
    }
}

// Fabrika s ideal'noy peredachey argumentov.
template <class T, class... Args>
[[nodiscard]] T make(Args&&... args) {
    return T(std::forward<Args>(args)...);
}

struct Gadget {
    Gadget(int n, std::string label) : count(n), name(std::move(label)) {}
    int count;
    std::string name;
};

}  // namespace

int main() {
    std::println("reference collapsing:");
    static_assert(std::is_lvalue_reference_v<int&>);
    static_assert(std::is_rvalue_reference_v<int&&>);
    std::println("  int  -> {}", ref_kind<int>());
    std::println("  int& -> {}", ref_kind<int&>());
    std::println("  int&& -> {}", ref_kind<int&&>());

    std::println("relay (category preserved):");
    int x = 5;
    relay(x);              // lvalue
    relay(std::move(x));   // rvalue
    relay(42);             // rvalue (prvalue)

    std::println("factory make<T>:");
    std::string text = make<std::string>("perfect forwarding");
    std::println("  string = '{}'", text);

    auto gadget = std::make_unique<Gadget>(3, std::string("widget"));
    std::println("  gadget: count={} name={}", gadget->count, gadget->name);

    std::cout << "perfect forwarding done\n";
    return (text == "perfect forwarding" && gadget->count == 3) ? 0 : 1;
}
