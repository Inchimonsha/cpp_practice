// m06_concurrency/ex05_coroutines — soprogrammy (coroutines).
// Standart: C++23. Zapusk: ./run.ps1 m06_concurrency/ex05_coroutines
#include <coroutine>
#include <exception>
#include <iostream>
#include <print>
#include <utility>
#include <vector>

namespace {

template <class T>
class Generator {
public:
    struct promise_type {
        T current{};
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current = std::move(value);
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() { std::terminate(); }
    };

    struct sentinel {};

    class iterator {
    public:
        explicit iterator(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
        iterator& operator++() { handle_.resume(); return *this; }
        const T& operator*() const { return handle_.promise().current; }
        bool operator==(sentinel) const { return handle_.done(); }
        bool operator!=(sentinel sent) const { return !(*this == sent); }
    private:
        std::coroutine_handle<promise_type> handle_;
    };

    explicit Generator(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    Generator(Generator&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    ~Generator() { if (handle_) { handle_.destroy(); } }

    iterator begin() { handle_.resume(); return iterator{handle_}; }
    sentinel end() { return {}; }

private:
    std::coroutine_handle<promise_type> handle_;
};

Generator<int> fibonacci(int count) {
    int a = 0;
    int b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;
        const int next = a + b;
        a = b;
        b = next;
    }
}

struct Ready {
    bool await_ready() const noexcept { return true; }
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    void await_resume() const noexcept {}
};

struct Task {
    struct promise_type {
        int value{};
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(int v) { value = v; }
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;
    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    Task(Task&& other) noexcept : handle(std::exchange(other.handle, {})) {}
    Task(const Task&) = delete;
    ~Task() { if (handle) { handle.destroy(); } }
    int result() const { return handle.promise().value; }
};

Task compute() {
    co_await Ready{};
    co_return 123;
}

}  // namespace

int main() {
    std::println("generator (co_yield):");
    std::vector<int> values;
    for (int v : fibonacci(8)) { values.push_back(v); }
    for (int v : values) { std::print(" {}", v); }
    std::print("\n");

    std::println("task (co_await/co_return):");
    Task t = compute();
    const int result = t.result();
    std::println("  result = {}", result);

    const std::vector<int> expected{0, 1, 1, 2, 3, 5, 8, 13};
    std::cout << "coroutines done\n";
    return (values == expected && result == 123) ? 0 : 1;
}
