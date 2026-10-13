// th05 — пример: std::array против std::vector. C++20.

#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace {

// Фиксированная таблица: направления (4 стороны).
const std::array<std::pair<int, int>, 4> kDirections{
    std::pair<int, int>{-1, 0},
    {1, 0},
    {0, -1},
    {0, 1},
};

std::string dir_name(const std::pair<int, int>& d) {
    if (d.first == -1) return "лево";
    if (d.first == 1) return "право";
    if (d.second == -1) return "верх";
    return "низ";
}

}  // namespace

int main() {
    std::cout << "std::array — направления:\n";
    for (const auto& d : kDirections) {
        std::cout << "  (" << d.first << ", " << d.second << ") — " << dir_name(d)
                  << '\n';
    }

    std::cout << "\nstd::vector — сбор пуль с reserve:\n";
    std::vector<int> bullets;
    bullets.reserve(8);  // предусматриваем 8 элементов
    for (int i = 0; i < 6; ++i) {
        bullets.push_back(i);
    }
    std::cout << "  size=" << bullets.size() << " capacity=" << bullets.capacity()
              << '\n';
    bullets.push_back(100);
    std::cout << "  после добавления: size=" << bullets.size()
              << " capacity=" << bullets.capacity() << '\n';

    std::cout << "\nemplace_back для std::string:\n";
    std::vector<std::string> names;
    names.reserve(3);
    names.emplace_back("PlayerOne");
    names.emplace_back(5, 'A');  // строка "AAAAA"
    for (const auto& name : names) {
        std::cout << "  '" << name << "'\n";
    }
    return 0;
}