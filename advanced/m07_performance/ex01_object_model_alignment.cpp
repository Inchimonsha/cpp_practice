// m07_performance/ex01_object_model_alignment — model' ob"ekta, vyravnivanie, bit_cast.
// Standart: C++23. Zapusk: ./run.ps1 m07_performance/ex01_object_model_alignment
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>

namespace {
struct A { char a; int b; char c; };
struct B { int b; char a; char c; };
struct alignas(16) Vec4 { float x, y, z, w; };
}  // namespace

int main() {
    std::println("layout:");
    std::println("  A:    sizeof={} alignof={}", sizeof(A), alignof(A));
    std::println("  B:    sizeof={} alignof={}", sizeof(B), alignof(B));
    std::println("  Vec4: sizeof={} alignof={}", sizeof(Vec4), alignof(Vec4));

    std::println("offsets in A:");
    std::println("  a={} b={} c={}", offsetof(A, a), offsetof(A, b), offsetof(A, c));

    std::println("bit_cast:");
    const float one = 1.0f;
    const auto bits = std::bit_cast<std::uint32_t>(one);
    std::println("  1.0f -> 0x{:08X}", bits);
    const float back = std::bit_cast<float>(bits);
    std::println("  back = {}", back);

    std::println("bytes <-> struct:");
    const Vec4 v{1.0f, 2.0f, 3.0f, 4.0f};
    const auto raw = std::bit_cast<std::array<std::byte, sizeof(Vec4)>>(v);
    const Vec4 restored = std::bit_cast<Vec4>(raw);
    std::println("  restored = ({}, {}, {}, {})",
                 restored.x, restored.y, restored.z, restored.w);

    std::cout << "object model done\n";
    return (bits == 0x3F800000u && back == 1.0f && restored.w == 4.0f) ? 0 : 1;
}
