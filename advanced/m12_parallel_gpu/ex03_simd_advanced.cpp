// m12_parallel_gpu/ex03_simd_advanced — simd-maski, where-vyrazheniya, reduce.
// Standart: C++23 (<experimental/simd>). Zapusk: ./run.ps1 m12_parallel_gpu/ex03_simd_advanced
#include <array>
#include <cstddef>
#include <experimental/simd>
#include <iostream>
#include <print>

namespace stdx = std::experimental;

using V = stdx::fixed_size_simd<float, 4>;
using M = V::mask_type;

int main() {
    const std::array<float, 4> raw{1.5f, -2.0f, 0.25f, -3.5f};
    const V input(raw.data(), stdx::element_aligned);
    const V zero(0.0f);
    const V one(1.0f);

    const V clamped = stdx::clamp(input, zero, one);

    const M negative = input < zero;
    V magnitude = input;
    stdx::where(negative, magnitude) = stdx::abs(input);

    const M big = stdx::abs(input) > V(2.0f);
    V indicator = zero;
    stdx::where(big, indicator) = one;
    const int count = static_cast<int>(stdx::reduce(indicator));

    const std::array<float, 4> left_raw{1.0f, 2.0f, 3.0f, 4.0f};
    const std::array<float, 4> right_raw{5.0f, 6.0f, 7.0f, 8.0f};
    const V left(left_raw.data(), stdx::element_aligned);
    const V right(right_raw.data(), stdx::element_aligned);
    const float dot = stdx::reduce(left * right);

    std::println("simd width = {}", V::size());
    std::print("clamp01 =");
    for (std::size_t index = 0; index < 4; ++index) {
        std::print(" {}", static_cast<float>(clamped[index]));
    }
    std::print("\nsign-abs =");
    for (std::size_t index = 0; index < 4; ++index) {
        std::print(" {}", static_cast<float>(magnitude[index]));
    }
    std::println("\ncount(|x| > 2) = {}", count);
    std::println("dot = {}", dot);

    const std::array<float, 4> expected_clamped{1.0f, 0.0f, 0.25f, 0.0f};
    const std::array<float, 4> expected_magnitude{1.5f, 2.0f, 0.25f, 3.5f};
    bool ok = count == 1 && dot == 70.0f;
    for (std::size_t index = 0; index < 4; ++index) {
        ok = ok && clamped[index] == expected_clamped[index];
        ok = ok && magnitude[index] == expected_magnitude[index];
    }
    std::cout << "simd advanced done\n";
    return ok ? 0 : 1;
}
