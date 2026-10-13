// fraction.hpp — объявления. #pragma once защищает от двойного включения.
// Пример правильного разделения на .hpp (объявления) и .cpp (определения).

#pragma once

namespace ex_fraction {

struct Fraction {
    int numerator;
    int denominator;  // не равен нулю
};

// Объявления (определения — в fraction.cpp):
Fraction make_fraction(int numerator, int denominator);
Fraction add(const Fraction& a, const Fraction& b);
double as_double(const Fraction& f);

}  // namespace ex_fraction