#include "mathx.hpp"

namespace mathx {

std::int64_t gcd(std::int64_t a, std::int64_t b) {
    if (a < 0) { a = -a; }
    if (b < 0) { b = -b; }
    while (b != 0) {
        const std::int64_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

std::int64_t add(std::int64_t a, std::int64_t b) { return a + b; }

std::int64_t mul(std::int64_t a, std::int64_t b) { return a * b; }

}  // namespace mathx
