// m15_systems/ex03_real_time — lock-free SPSC ring buffer i poryadok pamyati.
// Pokazyvaet determinizm (bez potokov): korrektnost' kol'cevogo bufera.
// Standart: C++23. Zapusk: ./run.ps1 m15_systems/ex03_real_time
#include <array>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <print>

namespace {

template <class T, std::size_t Capacity>
class SpscRing {
    static_assert(Capacity >= 2 && (Capacity & (Capacity - 1)) == 0,
                  "Capacity dolzhna byt' stepen'yu dvoyki");

public:
    bool push(const T& value) {
        const std::size_t head = head_.load(std::memory_order_relaxed);
        if (head - tail_.load(std::memory_order_acquire) == Capacity) {
            return false;
        }
        buffer_[head & kMask] = value;
        head_.store(head + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& out) {
        const std::size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) {
            return false;
        }
        out = buffer_[tail & kMask];
        tail_.store(tail + 1, std::memory_order_release);
        return true;
    }

    std::size_t size() const {
        return head_.load(std::memory_order_acquire) - tail_.load(std::memory_order_acquire);
    }

private:
    static constexpr std::size_t kMask = Capacity - 1;
    alignas(64) std::array<T, Capacity> buffer_{};
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
};

}  // namespace

int main() {
    SpscRing<int, 2048> queue;

    constexpr int kCount = 1000;
    int pushed = 0;
    for (int value = 1; value <= kCount; ++value) {
        if (queue.push(value)) {
            ++pushed;
        }
    }

    long long sum = 0;
    int popped = 0;
    bool ordered = true;
    int expected = 1;
    int value = 0;
    while (queue.pop(value)) {
        ordered = ordered && value == expected;
        sum += value;
        ++popped;
        ++expected;
    }
    std::println("pushed {}, popped {}, sum = {}", pushed, popped, sum);

    SpscRing<int, 4> small;
    bool full_rejected = false;
    for (int i = 0; i < 10; ++i) {
        if (!small.push(i)) {
            full_rejected = true;
            break;
        }
    }
    std::println("overflow rejected = {}", full_rejected ? "true" : "false");
    std::println("spsc order correct = {}", ordered ? "true" : "false");

    const bool ok = pushed == kCount && popped == kCount && sum == 500500 && ordered &&
                    full_rejected;
    std::cout << "real time done\n";
    return ok ? 0 : 1;
}
