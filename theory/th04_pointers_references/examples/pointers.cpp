// th04 — пример: указатели, nullptr, перепривязка. C++20.

#include <iostream>

namespace {

inline void describe(const int* value) {
    if (value == nullptr) {
        std::cout << "  (нет значения)\n";
    } else {
        std::cout << "  значение = " << *value << '\n';
    }
}

int max_or_default(const int* a, const int* b, int fallback) {
    if (a == nullptr && b == nullptr) {
        return fallback;
    }
    const int va = (a != nullptr) ? *a : fallback;
    const int vb = (b != nullptr) ? *b : fallback;
    return (va >= vb) ? va : vb;
}

}  // namespace

int main() {
    int health = 70;
    int energy = 40;

    int* current = &health;   // указывает на health
    describe(current);        // 70
    current = &energy;        // перепривязка — можно
    describe(current);        // 40

    current = nullptr;        // «значения нет»
    describe(current);

    std::cout << "max_or_default(nullptr, nullptr, 5) = "
              << max_or_default(nullptr, nullptr, 5) << '\n';
    std::cout << "max_or_default(&health, &energy, 5)  = "
              << max_or_default(&health, &energy, 5) << '\n';
    return 0;
}