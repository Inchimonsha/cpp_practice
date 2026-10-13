// th03 — пример: вызов функций из другого файла (fraction.hpp/fraction.cpp).
// C++20.

#include "fraction.hpp"

#include <iostream>

int main() {
    const ex_fraction::Fraction a = ex_fraction::make_fraction(1, 2);
    const ex_fraction::Fraction b = ex_fraction::make_fraction(1, 3);
    const ex_fraction::Fraction sum = ex_fraction::add(a, b);

    std::cout << "1/2 + 1/3 = " << sum.numerator << "/" << sum.denominator
              << " = " << ex_fraction::as_double(sum) << '\n';
    return 0;
}