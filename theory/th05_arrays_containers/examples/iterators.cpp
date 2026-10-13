// th05 — пример: итераторы, range-for, безопасный доступ. C++20.

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

void bump_all(std::vector<int>& values) {
    for (auto& x : values) {  // & — изменяем элементы
        x += 1;
    }
}

}  // namespace

int main() {
    std::vector<int> hp{10, 20, 30};

    std::cout << "range-for с изменением:\n";
    bump_all(hp);
    for (const auto x : hp) {
        std::cout << "  hp: " << x << (x == hp.back() ? "" : " ,");
    }
    std::cout << '\n';

    std::cout << "\nитераторы явно:\n";
    for (auto it = hp.begin(); it != hp.end(); ++it) {
        std::cout << "  " << *it << '\n';
    }

    std::cout << "\nat() vs []:\n";
    std::cout << "  hp.at(1) = " << hp.at(1) << '\n';
    try {
        (void)hp.at(99);  // out_of_range
    } catch (const std::out_of_range& e) {
        std::cout << "  hp.at(99) бросил out_of_range\n";
    }

    std::cout << "\nalgorithm (th12 — подробно):\n";
    const int found = *std::find(hp.begin(), hp.end(), 21);
    std::cout << "  найден элемент: " << found << '\n';
    return 0;
}