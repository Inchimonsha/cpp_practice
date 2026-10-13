// m15_systems/ex06_hardening_security — bezopasnye granicy, proverka perepolneniy.
// V real'nosti: -D_FORTIFY_SOURCE, -fstack-protector, ASLR/DEP, /GUARD:CF.
// Standart: C++23. Zapusk: ./run.ps1 m15_systems/ex06_hardening_security
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <print>
#include <span>
#include <string_view>

namespace {

std::size_t bounded_copy(std::span<char> destination, std::string_view source) {
    const std::size_t count = std::min(destination.size(), source.size());
    std::copy_n(source.begin(), count, destination.begin());
    return count;
}

void secure_erase(void* data, std::size_t size) {
    volatile unsigned char* bytes = static_cast<volatile unsigned char*>(data);
    for (std::size_t i = 0; i < size; ++i) {
        bytes[i] = 0;
    }
}

}  // namespace

int main() {
    char destination[8]{};
    const std::size_t copied = bounded_copy(destination, "hello");
    std::println("bounded copy: copied {}, dst = \"{}\"", copied,
                 std::string_view(destination, copied));

    const std::size_t big = std::numeric_limits<std::size_t>::max() - 3;
    std::size_t sum = 0;
    const bool overflow = __builtin_add_overflow(big, std::size_t{10}, &sum);
    std::println("integer overflow detected = {}", overflow ? "true" : "false");

    char secret[] = "top-secret";
    secure_erase(secret, sizeof(secret));
    const bool erased =
        std::all_of(std::begin(secret), std::end(secret), [](char byte) { return byte == 0; });
    std::println("secure erase verified = {}", erased ? "true" : "false");

    const int values[] = {1, 2, 3};
    const std::span<const int> view = values;
    std::println("span size = {}", view.size());

    const bool ok = copied == 5 && overflow && erased && view.size() == 3;
    std::cout << "hardening security done\n";
    return ok ? 0 : 1;
}
