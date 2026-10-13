// th05 — пример: std::span — единый интерфейс для разных контейнеров. C++20.

#include <algorithm>
#include <array>
#include <iostream>
#include <span>
#include <vector>

namespace {

double average(std::span<const int> data) {
    if (data.empty()) {
        return 0.0;
    }
    long long sum = 0;
    for (const int x : data) {
        sum += x;
    }
    return static_cast<double>(sum) / static_cast<double>(data.size());
}

int max_of(std::span<const int> data) {
    return data.empty() ? 0 : *std::max_element(data.begin(), data.end());
}

}  // namespace

int main() {
    std::array<int, 4> arr{1, 2, 3, 4};
    std::vector<int> vec{10, 5, 20};
    const int c_arr[] = {7, 7, 7};

    std::cout << "std::array : avg=" << average(arr) << " max=" << max_of(arr)
              << '\n';
    std::cout << "std::vector: avg=" << average(vec) << " max=" << max_of(vec)
              << '\n';
    std::cout << "C-массив   : avg=" << average(c_arr) << " max=" << max_of(c_arr)
              << '\n';
    std::cout << "пусто      : avg=" << average({}) << " max=" << max_of({})
              << '\n';
    return 0;
}