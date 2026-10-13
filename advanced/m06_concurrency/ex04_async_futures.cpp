// m06_concurrency/ex04_async_futures — std::async, future, promise, packaged_task.
// Standart: C++23. Zapusk: ./run.ps1 m06_concurrency/ex04_async_futures
#include <future>
#include <iostream>
#include <print>
#include <thread>

int main() {
    std::println("std::async:");
    auto f = std::async(std::launch::async, [] { return 6 * 7; });
    const int async_result = f.get();
    std::println("  result = {}", async_result);

    std::println("promise/future:");
    std::promise<int> p;
    auto fut = p.get_future();
    std::thread t([&p] { p.set_value(99); });
    t.join();
    const int promised = fut.get();
    std::println("  value = {}", promised);

    std::println("packaged_task:");
    std::packaged_task<int(int)> task([](int x) { return x * x; });
    auto tf = task.get_future();
    std::thread t2(std::move(task), 7);
    t2.join();
    const int square = tf.get();
    std::println("  square = {}", square);

    std::println("deferred:");
    auto d = std::async(std::launch::deferred, [] {
        std::println("  (deferred body runs now)");
        return 5;
    });
    const int deferred_result = d.get();
    std::println("  deferred result = {}", deferred_result);

    std::cout << "async/futures done\n";
    const bool ok = async_result == 42 && promised == 99 && square == 49 &&
                    deferred_result == 5;
    return ok ? 0 : 1;
}
