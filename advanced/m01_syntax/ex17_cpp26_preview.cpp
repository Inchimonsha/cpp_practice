// m01_syntax/ex17_cpp26_preview — obzor C++26.
// Standart: C++26 (preview). Zapusk: ./run.ps1 m01_syntax/ex17_cpp26_preview -Std c++26
// Nekotorye vozmozhnosti trebuyut -fcontracts; skript dobavlyaet ih po neobhodimosti.
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <print>
#include <string>
#include <tuple>
#include <version>

#if __has_include(<inplace_vector>)
#  include <inplace_vector>
#  define HAVE_INPLACE_VECTOR 1
#else
#  define HAVE_INPLACE_VECTOR 0
#endif

namespace {

#if defined(__cpp_pack_indexing)
template <typename... Ts>
[[nodiscard]] auto first(Ts... args) {
    return args...[0];  // C++26: indeksaciya paketa
}
#else
template <typename... Ts>
[[nodiscard]] auto first(Ts... args) {
    return std::get<0>(std::forward_as_tuple(args...));
}
#endif

#if defined(__cpp_contracts)
int clamp_nonnegative(int x)
    pre(x >= 0)
    post(r : r >= 0)
{
    return x;
}
#endif

struct NoCopy {
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete("NoCopy is not copyable");  // prichina udaleniya
};

void report_macros() {
#if defined(__cpp_contracts)
    std::println("contracts        : {}", __cpp_contracts);
#else
    std::println("contracts        : n/a");
#endif
#if defined(__cpp_pack_indexing)
    std::println("pack indexing    : {}", __cpp_pack_indexing);
#else
    std::println("pack indexing    : n/a");
#endif
#if HAVE_INPLACE_VECTOR
    std::println("inplace_vector   : {}", __cpp_lib_inplace_vector);
#else
    std::println("inplace_vector   : n/a");
#endif
#if defined(__cpp_lib_senders)
    std::println("senders          : {}", __cpp_lib_senders);
#else
    std::println("senders          : n/a (ozhidaetsya v budushchih GCC)");
#endif
#if defined(__cpp_reflection)
    std::println("reflection       : {}", __cpp_reflection);
#else
    std::println("reflection       : n/a (eksperimental'no, -freflection)");
#endif
}

}  // namespace

int main() {
    std::println("__cplusplus = {}", __cplusplus);
    report_macros();

#if HAVE_INPLACE_VECTOR
    std::inplace_vector<int, 4> values;
    values.push_back(10);
    values.push_back(20);
    values.push_back(30);
    std::println("inplace_vector size={} capacity={}", values.size(), values.capacity());
#else
    std::println("inplace_vector unavailable in this toolchain");
#endif

    std::println("first(1,2,3) = {}", first(1, 2, 3));

#if defined(__cpp_contracts)
    std::println("clamp_nonnegative(5) = {}", clamp_nonnegative(5));
#else
    std::println("contracts unavailable in this toolchain");
#endif

    const NoCopy a;
    (void)a;
    std::cout << "cpp26 preview done\n";
    return 0;
}
