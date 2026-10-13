// m05_errors/ex01_exceptions_safety — garantii bezopasnosti isklyuchenij.
// Standart: C++23. Zapusk: ./run.ps1 m05_errors/ex01_exceptions_safety
#include <iostream>
#include <print>
#include <stdexcept>
#include <vector>

namespace {

struct Failure : std::runtime_error {
    Failure() : std::runtime_error("simulated failure") {}
};

// Simulyaciya sboya pri opredelennom znachenii.
void maybe_throw(int value) {
    if (value == 13) { throw Failure{}; }
}

class Bag {
public:
    void add_pair_strong(int a, int b) {
        std::vector<int> next = data_;   // kopiya
        next.push_back(a);
        maybe_throw(b);                  // brosit -> data_ ne tronut
        next.push_back(b);
        data_.swap(next);                // noexcept
    }

    void add_pair_basic(int a, int b) {
        data_.push_back(a);              // uspelo
        maybe_throw(b);                  // brosit -> chastichnyj rezul'tat
        data_.push_back(b);
    }

    void swap(Bag& other) noexcept { data_.swap(other.data_); }

    [[nodiscard]] std::vector<int> values() const { return data_; }

private:
    std::vector<int> data_{1, 2};
};

void dump_values(const char* label, const std::vector<int>& v) {
    std::print("{}:", label);
    if (v.empty()) { std::print(" <empty>"); }
    for (int x : v) { std::print(" {}", x); }
    std::print("\n");
}

}  // namespace

int main() {
    std::println("strong guarantee:");
    Bag strong;
    try {
        strong.add_pair_strong(10, 13);   // brosit na vtorom elemente
    } catch (const Failure& e) {
        std::println("  caught: {}", e.what());
    }
    dump_values("  after failed strong", strong.values());

    std::println("basic guarantee:");
    Bag basic;
    try {
        basic.add_pair_basic(10, 13);     // 10 dobavlen, 13 net
    } catch (const Failure& e) {
        std::println("  caught: {}", e.what());
    }
    dump_values("  after failed basic", basic.values());

    std::println("strong success path:");
    strong.add_pair_strong(10, 11);
    dump_values("  after success", strong.values());

    std::cout << "exceptions safety done\n";
    return (strong.values() == std::vector<int>{1, 2, 10, 11}) ? 0 : 1;
}
