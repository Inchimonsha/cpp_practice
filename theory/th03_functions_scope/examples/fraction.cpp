// fraction.cpp — определения функций, объявленных в fraction.hpp.

#include "fraction.hpp"

#include <stdexcept>

namespace ex_fraction {

Fraction make_fraction(int numerator, int denominator) {
    if (denominator == 0) {
        throw std::invalid_argument("знаменатель не может быть нулём");
    }
    return Fraction{numerator, denominator};
}

Fraction add(const Fraction& a, const Fraction& b) {
    return Fraction{a.numerator * b.denominator + b.numerator * a.denominator,
                    a.denominator * b.denominator};
}

double as_double(const Fraction& f) {
    return static_cast<double>(f.numerator) / f.denominator;
}

}  // namespace ex_fraction