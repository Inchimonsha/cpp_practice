// m09_practice/ex07_arena_allocator — bump/arena-allocator i std::pmr.
// Standart: C++23. Zapusk: ./run.ps1 m09_practice/ex07_arena_allocator
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <memory_resource>
#include <new>
#include <print>
#include <utility>
#include <vector>

namespace {

struct Vec2 { float x; float y; };

class Arena {
public:
    explicit Arena(std::size_t capacity)
        : buffer_(std::make_unique<std::byte[]>(capacity)), capacity_(capacity) {}

    void* allocate(std::size_t size,
                   std::size_t alignment = alignof(std::max_align_t)) {
        void* ptr = buffer_.get() + offset_;
        std::size_t space = capacity_ - offset_;
        void* aligned = std::align(alignment, size, ptr, space);
        if (aligned == nullptr) { throw std::bad_alloc(); }
        offset_ = static_cast<std::size_t>(
                      static_cast<std::byte*>(aligned) - buffer_.get()) + size;
        return aligned;
    }

    template <class T, class... Args>
    T* create(Args&&... args) {
        return std::construct_at(reinterpret_cast<T*>(allocate(sizeof(T), alignof(T))),
                                 std::forward<Args>(args)...);
    }

    void reset() { offset_ = 0; }
    std::size_t used() const { return offset_; }
    std::size_t capacity() const { return capacity_; }

private:
    std::unique_ptr<std::byte[]> buffer_;
    std::size_t capacity_;
    std::size_t offset_ = 0;
};

bool aligned_as(const void* ptr, std::size_t alignment) {
    return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

}  // namespace

int main() {
    Arena arena(256);
    std::println("capacity = {}", arena.capacity());

    int* number = arena.create<int>(42);
    std::println("after int: used = {}", arena.used());

    double* ratio = arena.create<double>(3.5);
    std::println("after double: used = {}", arena.used());

    Vec2* point = arena.create<Vec2>(1.0f, 2.0f);
    std::println("after Vec2: used = {}", arena.used());

    std::println("int aligned = {}", aligned_as(number, alignof(int)));
    std::println("double aligned = {}", aligned_as(ratio, alignof(double)));
    std::println("value = {} {} ({}, {})", *number, *ratio, point->x, point->y);

    const std::size_t used_before_reset = arena.used();
    arena.reset();
    std::println("after reset: used = {}", arena.used());

    std::array<std::byte, 1024> buffer{};
    std::pmr::monotonic_buffer_resource resource(buffer.data(), buffer.size());
    std::pmr::vector<int> values(&resource);
    long long sum = 0;
    for (int i = 0; i < 100; ++i) {
        values.push_back(i);
        sum += i;
    }
    const auto* first = reinterpret_cast<const std::byte*>(values.data());
    const bool in_buffer = first >= buffer.data() && first < buffer.data() + buffer.size();
    std::println("pmr sum = {}, in buffer = {}", sum, in_buffer);

    const bool ok = used_before_reset == 24 &&
                    aligned_as(number, alignof(int)) &&
                    aligned_as(ratio, alignof(double)) &&
                    arena.used() == 0 && sum == 4950 && in_buffer;
    std::cout << "arena allocator done\n";
    return ok ? 0 : 1;
}
