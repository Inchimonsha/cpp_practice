// m06_concurrency/ex02_mutex_locks_cv — mutex, loki, condition_variable.
// Standart: C++23. Zapusk: ./run.ps1 m06_concurrency/ex02_mutex_locks_cv
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <print>
#include <queue>
#include <thread>
#include <vector>

int main() {
    constexpr int kThreads = 4;
    constexpr int kPer = 1000;

    std::println("mutex-protected counter:");
    std::mutex m;
    long long counter = 0;
    {
        std::vector<std::thread> pool;
        for (int t = 0; t < kThreads; ++t) {
            pool.emplace_back([&] {
                for (int i = 0; i < kPer; ++i) {
                    std::lock_guard<std::mutex> g(m);
                    ++counter;
                }
            });
        }
        for (auto& th : pool) { th.join(); }
    }
    std::println("  counter = {}", counter);

    std::println("scoped_lock (two mutexes, no deadlock):");
    std::mutex a;
    std::mutex b;
    long long ops = 0;
    auto worker = [&](int times) {
        for (int i = 0; i < times; ++i) {
            std::scoped_lock lock(a, b);
            ++ops;
        }
    };
    { std::thread t1(worker, 500); std::thread t2(worker, 500); t1.join(); t2.join(); }
    std::println("  ops = {}", ops);

    std::println("condition_variable producer/consumer:");
    std::mutex qm;
    std::condition_variable qcv;
    std::queue<int> q;
    bool done = false;
    std::thread producer([&] {
        for (int i = 1; i <= 5; ++i) {
            { std::lock_guard<std::mutex> g(qm); q.push(i); }
            qcv.notify_one();
        }
        { std::lock_guard<std::mutex> g(qm); done = true; }
        qcv.notify_all();
    });
    long long sum = 0;
    std::thread consumer([&] {
        for (;;) {
            std::unique_lock<std::mutex> lock(qm);
            qcv.wait(lock, [&] { return !q.empty() || done; });
            if (!q.empty()) {
                sum += q.front();
                q.pop();
            } else if (done) {
                break;
            }
        }
    });
    producer.join();
    consumer.join();
    std::println("  sum = {}", sum);

    std::cout << "mutex/locks/cv done\n";
    return (counter == 4000 && ops == 1000 && sum == 15) ? 0 : 1;
}
