// th02 — пример: размеры и диапазоны базовых типов.
// Показывает, почему нельзя «зашивать» размеры в код и как полагаться
// на std::numeric_limits. C++20.

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

int main() {
    std::cout << "sizeof - размеры типов на этой платформе:\n";
    std::cout << "  bool        : " << sizeof(bool) << " байт\n";
    std::cout << "  char        : " << sizeof(char) << " байт\n";
    std::cout << "  int         : " << sizeof(int) << " байт\n";
    std::cout << "  unsigned int: " << sizeof(unsigned int) << " байт\n";
    std::cout << "  float       : " << sizeof(float) << " байт\n";
    std::cout << "  double      : " << sizeof(double) << " байт\n";
    std::cout << "  int64_t     : " << sizeof(std::int64_t) << " байт\n";

    std::cout << "\nstd::numeric_limits - диапазоны:\n";
    using lim_int = std::numeric_limits<int>;
    using lim_uint = std::numeric_limits<unsigned int>;
    using lim_float = std::numeric_limits<float>;
    std::cout << "  int         : [" << lim_int::min() << " .. "
              << lim_int::max() << "]\n";
    std::cout << "  unsigned int: [" << lim_uint::min() << " .. "
              << lim_uint::max() << "]\n";

    std::cout << std::setprecision(9);
    std::cout << "  float       : " << std::numeric_limits<float>::digits10
              << " десятичных знаков; min: " << lim_float::min()
              << ", max: " << lim_float::max() << "\n";

    // Демонстрация: деление целых и остаток.
    const int a = 7;
    const int b = 2;
    std::cout << "\nцелочисленная арифметика:\n";
    std::cout << "  7 / 2 == " << (a / b) << ";  7.0 / 2.0 == " << (7.0 / 2.0)
              << ";  7 % 2 == " << (a % b) << '\n';

    // Опасный пример: отключён выводом, чтобы пример не «падал».
    // Неинициализированная переменная + UB читать нельзя даже в учебных целях.
    const unsigned int zero = 0u;
    std::cout << "  0u - 1 == " << (zero - 1u) << "  (модулярная арифметика!)\n";
    return 0;
}