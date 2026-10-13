// m10_language_future/ex05_simd_hive_containers — SIMD i beskoechno-prostranstvennye konteinery.
// C++26: std::simd i std::hive v GCC 16.2 net -> <experimental/simd> i std::inplace_vector.
#include <array>
#include <cstddef>
#include <experimental/simd>
#include <inplace_vector>
#include <iostream>
#include <print>

namespace stdx = std::experimental;

void axpy(float a, const float* x, const float* y, float* out, std::size_t n) {
    using V = stdx::native_simd<float>;
    constexpr std::size_t width = static_cast<std::size_t>(V::size());
    std::size_t i = 0;
    for (; i + width <= n; i += width) {
        const V vx(x + i, stdx::element_aligned);
        const V vy(y + i, stdx::element_aligned);
        (a * vx + vy).copy_to(out + i, stdx::element_aligned);
    }
    for (; i < n; ++i) {
        out[i] = a * x[i] + y[i];
    }
}

int main() {
    std::println("simd width (native float) = {}", stdx::native_simd<float>::size());
    std::println("fixed_size_simd<float,4>::size() = {}",
                 stdx::fixed_size_simd<float, 4>::size());

    const std::array<float, 9> x{1, 2, 3, 4, 5, 6, 7, 8, 9};
    const std::array<float, 9> y{10, 20, 30, 40, 50, 60, 70, 80, 90};
    std::array<float, 9> out{};
    axpy(2.0f, x.data(), y.data(), out.data(), x.size());

    bool axpy_ok = true;
    for (std::size_t i = 0; i < x.size(); ++i) {
        if (out[i] != 2.0f * x[i] + y[i]) {
            axpy_ok = false;
        }
    }
    std::println("axpy ok = {}", axpy_ok);

    const std::array<float, 4> quad{1.0f, 2.0f, 3.0f, 4.0f};
    const stdx::fixed_size_simd<float, 4> fixed(quad.data(), stdx::element_aligned);
    std::println("reduce(1,2,3,4) = {}", static_cast<float>(stdx::reduce(fixed)));

    std::inplace_vector<int, 4> buffer;
    for (const int value : {3, 6, 9}) {
        buffer.push_back(value);
    }
    std::println("inplace_vector size={} capacity={}", buffer.size(), buffer.capacity());

    bool rejected = false;
    buffer.push_back(12);
    try {
        buffer.push_back(99);
    } catch (const std::bad_alloc&) {
        rejected = true;
    }
    std::println("overflow rejected = {} (size={})", rejected, buffer.size());

    const bool buffer_ok = buffer.size() == 4 && buffer[0] == 3 && buffer[3] == 12 &&
                           buffer.capacity() == 4;

    std::cout << "simd/hive containers done\n";
    return (axpy_ok && buffer_ok && rejected) ? 0 : 1;
}
