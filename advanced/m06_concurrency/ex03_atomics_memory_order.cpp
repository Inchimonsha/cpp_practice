// m06_concurrency/ex03_atomics_memory_order — atomiki i model' pamyati.
// Standart: C++23. Zapusk: ./run.ps1 m06_concurrency/ex03_atomics_memory_order
#include <atomic>
#include <iostream>
#include <print>
#include <thread>
#include <vector>

int main() {
    std::println("atomics increment:");
    std::atomic<int> counter{0};
    {
        std::vector<std::thread> pool;
        for (int t = 0; t < 4; ++t) {
            pool.emplace_back([&counter] {
                for (int i = 0; i < 1000; ++i) {
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& th : pool) { th.join(); }
    }
    std::println("  counter = {}", counter.load());

    std::println("lock-free check:");
    std::atomic<int> probe_i{0};
    std::atomic<long long> probe_ll{0};
    std::println("  atomic<int>  is_lock_free = {}", probe_i.is_lock_free());
    std::println("  atomic<long long> is_lock_free = {}", probe_ll.is_lock_free());

    std::println("spinlock via atomic_flag:");
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    long long protected_value = 0;
    auto spin = [&](int n) {
        for (int i = 0; i < n; ++i) {
            while (flag.test_and_set(std::memory_order_acquire)) {}
            ++protected_value;
            flag.clear(std::memory_order_release);
        }
    };
    { std::thread t1(spin, 5000); std::thread t2(spin, 5000); t1.join(); t2.join(); }
    std::println("  protected_value = {}", protected_value);

    std::println("release/acquire handoff:");
    int data = 0;
    std::atomic<bool> ready{false};
    int observed = 0;
    std::thread producer([&] {
        data = 42;
        ready.store(true, std::memory_order_release);
    });
    std::thread consumer([&] {
        while (!ready.load(std::memory_order_acquire)) {}
        observed = data;
    });
    producer.join();
    consumer.join();
    std::println("  observed = {}", observed);

    std::cout << "atomics done\n";
    return (counter.load() == 4000 && protected_value == 10000 && observed == 42)
               ? 0
               : 1;
}
