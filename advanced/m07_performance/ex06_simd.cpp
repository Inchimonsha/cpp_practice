// m07_performance/ex06_simd — vektorizaciya i std::simd.
// Standart: C++23. Zapusk: ./run.ps1 m07_performance/ex06_simd
// Podskazka: dobav'te -march=native cherez -Extra dlya shirokogo native_simd.
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <version>

#include <experimental/simd>   // GNU; v C++26 ozhidaetsya <simd>

int main() {
#if defined(__cpp_lib_simd)
    std::println("std::simd: __cpp_lib_simd = {}",
                 static_cast<long>(__cpp_lib_simd));
#else
    std::println("std::simd: not available (GCC 16.2), using <experimental/simd>");
#endif

    namespace ex = std::experimental;
    using V4 = ex::fixed_size_simd<float, 4>;
    std::println("fixed_size_simd<float,4>::size() = {}", V4::size());
    std::println("native_simd<float>::size() = {}", ex::native_simd<float>::size());

    const std::array<float, 4> a{1.0f, 2.0f, 3.0f, 4.0f};
    const std::array<float, 4> b{5.0f, 6.0f, 7.0f, 8.0f};

    V4 va(0.0f);
    V4 vb(0.0f);
    for (std::size_t i = 0; i < 4; ++i) {
        va[i] = a[i];
        vb[i] = b[i];
    }

    const float dot = ex::reduce(va * vb);

    float scalar = 0.0f;
    for (std::size_t i = 0; i < 4; ++i) { scalar += a[i] * b[i]; }

    std::println("scalar dot = {}", scalar);
    std::println("simd   dot = {}", dot);

    std::cout << "simd done\n";
    return (scalar == dot && dot == 70.0f) ? 0 : 1;
}
