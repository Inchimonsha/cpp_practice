// m03_templates/ex02_specialization_sfinae — specializaciya i SFINAE.
// Standart: C++23. Zapusk: ./run.ps1 m03_templates/ex02_specialization_sfinae
#include <iostream>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

namespace {

// Osnovnoy shablon.
template <class T>
struct describe {
    static const char* text() { return "value"; }
};

// Chastichnaya specializaciya dlya ukazateley.
template <class T>
struct describe<T*> {
    static const char* text() { return "pointer"; }
};

// Polnaya specializaciya dlya bool.
template <>
struct describe<bool> {
    static const char* text() { return "boolean"; }
};

// Chastichnaya specializaciya dlya vector<T>.
template <class T>
struct describe<std::vector<T>> {
    static const char* text() { return "vector"; }
};

// SFINAE: funkciya est' tol'ko dlya celochislennyh tipov.
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
std::string only_integral(T value) {
    return "integral:" + std::to_string(value);
}

// Tag dispatch: vybor vetki po priznaku tipa.
template <class T>
std::string stringify_impl(T value, std::true_type) {
    return std::to_string(value);
}

template <class T>
std::string stringify_impl(T, std::false_type) {
    return "<not stringifiable>";
}

template <class T>
std::string stringify(T value) {
    return stringify_impl(value, std::is_integral<T>{});
}

}  // namespace

int main() {
    std::println("describe<int>          = {}", describe<int>::text());
    std::println("describe<bool>         = {}", describe<bool>::text());
    std::println("describe<int*>         = {}", describe<int*>::text());
    std::println("describe<vector<int>>  = {}", describe<std::vector<int>>::text());

    std::println("only_integral(42) = {}", only_integral(42));
    // only_integral(2.5);  // oshibka kompilyacii: double ne celochislennyj

    std::println("stringify(7)    = {}", stringify(7));
    std::println("stringify(2.5)  = {}", stringify(2.5));

    std::cout << "specialization/sfinae done\n";
    return std::string(describe<bool>::text()) == "boolean" ? 0 : 1;
}
