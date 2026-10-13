// m06_concurrency/ex01_threads_jthread — potoki i std::jthread.
// Standart: C++23. Zapusk: ./run.ps1 m06_concurrency/ex01_threads_jthread
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <print>
#include <thread>
#include <vector>

namespace {
constexpr int kThreads = 4;
constexpr int kPerThread = 1000;
}  // namespace

int main() {
    std::println("std::thread + atomic:");
    std::atomic<int> total{0};
    {
        std::vector<std::thread> pool;
        for (int t = 0; t < kThreads; ++t) {
            pool.emplace_back([&total] {
                for (int i = 0; i < kPerThread; ++i) {
                    total.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& th : pool) { th.join(); }
    }
    std::println("  total = {} (expected {})", total.load(), kThreads * kPerThread);

    std::println("std::thread partial sums:");
    std::vector<long long> partial(kThreads, 0);
    {
        std::vector<std::thread> pool;
        for (int t = 0; t < kThreads; ++t) {
            pool.emplace_back([&partial, t] {
                long long sum = 0;
                for (int i = 1; i <= kPerThread; ++i) { sum += i; }
                partial[static_cast<std::size_t>(t)] = sum;
            });
        }
        for (auto& th : pool) { th.join(); }
    }
    long long grand = 0;
    for (long long p : partial) { grand += p; }
    const long long expected = static_cast<long long>(kThreads) *
                               (kPerThread * (kPerThread + 1) / 2);
    std::println("  grand = {} (expected {})", grand, expected);

    std::println("std::jthread cooperative cancel:");
    std::mutex m;
    std::condition_variable_any cv;
    bool cancelled = false;
    std::jthread worker([&](std::stop_token st) {
        std::unique_lock lock(m);
        cv.wait(lock, st, [] { return false; });  // zhdem request_stop
        cancelled = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    worker.request_stop();
    worker.join();
    std::println("  cancelled = {}", cancelled);

    std::cout << "threads/jthread done\n";
    return (total.load() == kThreads * kPerThread && grand == expected && cancelled)
               ? 0
               : 1;
}
