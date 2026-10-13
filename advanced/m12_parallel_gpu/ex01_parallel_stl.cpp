// m12_parallel_gpu/ex01_parallel_stl — execution policies i parallel'nye algoritmy.
// Standart: C++23. Zapusk: ./run.ps1 m12_parallel_gpu/ex01_parallel_stl
#include <algorithm>
#include <atomic>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <print>
#include <vector>

int main() {
#if defined(__cpp_lib_execution)
    std::println("__cpp_lib_execution = {}", static_cast<long>(__cpp_lib_execution));
#endif
#if defined(__cpp_lib_parallel_algorithm)
    std::println("__cpp_lib_parallel_algorithm = {}",
                 static_cast<long>(__cpp_lib_parallel_algorithm));
#endif
#if defined(_PSTL_PAR_BACKEND_TBB)
    std::println("parallel backend: TBB");
#else
    std::println("parallel backend: serial fallback (TBB net)");
#endif

    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 1);

    std::vector<long long> squares(data.size());
    std::transform(std::execution::par, data.begin(), data.end(), squares.begin(),
                   [](int value) { return static_cast<long long>(value) * value; });
    const long long sum = std::reduce(std::execution::par, squares.begin(), squares.end(), 0LL);
    std::println("sum of squares 1..1000 = {}", sum);

    const long long map_reduce =
        std::transform_reduce(std::execution::par, data.begin(), data.end(), 0LL,
                              std::plus<>{},
                              [](int value) { return static_cast<long long>(value) * value; });
    std::println("transform_reduce sum = {}", map_reduce);

    std::atomic<int> even_count{0};
    std::for_each(std::execution::par_unseq, data.begin(), data.end(),
                  [&even_count](int value) {
                      if (value % 2 == 0) {
                          even_count.fetch_add(1, std::memory_order_relaxed);
                      }
                  });
    std::println("even count = {}", even_count.load());

    const auto big = std::count_if(std::execution::par, data.begin(), data.end(),
                                   [](int value) { return value > 500; });
    std::println("count > 500 = {}", big);

    std::vector<int> descending(100);
    std::iota(descending.begin(), descending.end(), 1);
    std::sort(std::execution::par_unseq, descending.begin(), descending.end(),
              std::greater<>{});
    std::println("sorted desc: front={} back={}", descending.front(), descending.back());

    const bool ok = sum == 333833500 && map_reduce == 333833500 && even_count.load() == 500 &&
                    big == 500 && descending.front() == 100 && descending.back() == 1;
    std::cout << "parallel stl done\n";
    return ok ? 0 : 1;
}
