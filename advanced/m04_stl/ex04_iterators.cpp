// m04_stl/ex04_iterators — iteratory.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex04_iterators
#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <print>
#include <type_traits>
#include <vector>

namespace {

// Prostaya posledovatel'nost': start, start+step, ... do (ne vklyuchaya) stop.
struct Range {
    int start{};
    int stop{};
    int step{1};

    struct Iterator {
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using iterator_concept = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using pointer = const int*;
        using reference = int;

        const Range* range_{};
        int current_{};

        int operator*() const { return current_; }

        Iterator& operator++() {
            current_ += range_->step;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return current_ == other.current_; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    [[nodiscard]] Iterator begin() const { return Iterator{this, start}; }
    [[nodiscard]] Iterator end() const { return Iterator{this, stop}; }
};

static_assert(std::forward_iterator<Range::Iterator>);

}  // namespace

int main() {
    std::vector<int> data{10, 20, 30, 40, 50, 60};

    auto it = data.begin();
    std::advance(it, 2);
    std::println("advance(begin, 2) -> {}", *it);
    std::println("next(begin)       -> {}", *std::next(data.begin()));
    std::println("prev(end, 2)      -> {}", *std::prev(data.end(), 2));

    std::vector<int> mirrored;
    std::copy(data.begin(), data.begin() + 3, std::back_inserter(mirrored));
    std::print("copied first 3:");
    for (int v : mirrored) { std::print(" {}", v); }
    std::print("\n");

    using Cat = std::iterator_traits<std::vector<int>::iterator>::iterator_category;
    std::println("vector iterator is random_access? {}",
                 std::is_same_v<Cat, std::random_access_iterator_tag>);

    std::println("custom range (0..10 step 2):");
    const Range range{0, 10, 2};
    std::print("  values:");
    for (int v : range) { std::print(" {}", v); }
    std::print("\n");

    int total = 0;
    for (Range::Iterator i = range.begin(); i != range.end(); ++i) {
        total += *i;
    }
    std::println("  manual sum = {}", total);

    std::cout << "iterators done\n";
    return total == 20 ? 0 : 1;
}
