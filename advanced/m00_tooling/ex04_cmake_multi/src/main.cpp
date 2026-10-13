#include <iostream>

#include "mathx.hpp"

int main() {
    const std::int64_t a = 1071;
    const std::int64_t b = 462;
    std::cout << "gcd(" << a << ", " << b << ") = " << mathx::gcd(a, b) << '\n';
    return 0;
}
