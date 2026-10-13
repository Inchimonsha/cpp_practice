// th03 — пример: области видимости, анонимный namespace, static. C++20.

#include <iostream>
#include <string>

namespace {

// Внутренние детали файла: видны только здесь.
constexpr int kBaseDx = 2;

int linear_step(int level) {
    return level * kBaseDx;
}

}  // namespace

// static в функции: одна копия на всю программу.
std::string next_unit_id() {
    static int next = 0;
    ++next;
    return "unit_" + std::to_string(next);
}

int main() {
    std::cout << "анонимный namespace (kBaseDx, linear_step):\n";
    std::cout << "  linear_step(3) = " << linear_step(3) << '\n';

    std::cout << "\nstatic-счётчик next_unit_id:\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "  " << next_unit_id() << '\n';
    }

    std::cout << "\nблочная видимость:\n";
    {
        const int block_value = 7;  // живёт только в этом блоке
        std::cout << "  внутри блока: block_value = " << block_value << '\n';
    }
    // block_value здесь недоступен — и это хорошо.
    std::cout << "  после блока block_value недоступен (компилятор не пустит)\n";
    return 0;
}