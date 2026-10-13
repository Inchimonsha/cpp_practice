# ex05_coroutines — сопрограммы (coroutines)

## Теория

Сопрограмма — функция, которая может **приостанавливаться** и
**возобновляться**, сохраняя состояние между вызовами. В C++ (C++20) это
безстековые (stackless) сопрограммы: компилятор превращает тело функции с
ключевыми словами `co_await`/`co_yield`/`co_return` в конечный автомат, а
состояние хранится в **кадре** (coroutine frame) в куче.

Три ключевых слова:

- `co_await expr` — приостановиться, пока `expr` (awaitable) не будет готов;
  `await_ready`, `await_suspend`, `await_resume` определяют поведение.
- `co_yield value` — выдать значение наружу и приостановиться (генератор).
- `co_return value` — штатно завершить сопрограмму.

Функция становится сопрограммой, только если содержит одно из этих слов.
Возвращаемый тип задаёт «шов» через вложенный `promise_type`: он управляет
начальной/конечной приостановкой, хранением значения и исключениями.

## Синтаксис

```cpp
struct Task {
    struct promise_type {
        int value;
        Task get_return_object();
        std::suspend_never initial_suspend() noexcept;
        std::suspend_always final_suspend() noexcept;
        void return_value(int v);
        void unhandled_exception();
    };
    // iterator/destroy dlya korytishka
};

Task f() {
    co_await SomeAwaitable{};
    co_return 42;
}
```

## Подводные камни

- Сопрограмма выполняется лишь когда её **возобновляют** (`resume()` или
  пробег итератором). Забыли возобновить — тело не выполнилось.
- Кадр размещается в куче: утечка, если никто не вызовет `destroy()`.
- `initial_suspend` = `suspend_always` (ленивая) или `suspend_never` (жадная) —
  выбор меняет момент начала выполнения.
- Тип должен предоставить `promise_type`; иначе «… cannot be used as a
  coroutine».
- В C++20 нет библиотечных `generator`/`task` — их пишут вручную. В C++23
  появился `std::generator`.

## Задача

1. Реализовать `Generator<T>` через `co_yield` и ленивый `initial_suspend`:
   функция `fibonacci(8)` выдаёт 0 1 1 2 3 5 8 13.
2. Реализовать `Task`, поддерживающий `co_await` и `co_return`: сопрограмма
   `compute()` делает `co_await Ready{}` и `co_return 123`.

## Где применяется

- Ленивые последовательности, асинхронный ввод-вывод, конечные автоматы,
  задачи с приостановкой, движки (корутины-скрипты), обработка потоков данных.

## Проверка

Вывод: последовательность Фибоначчи `0 1 1 2 3 5 8 13` и `result = 123`; код
возврата `0` при совпадении, иначе `1`.

## Решение

```cpp
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
```
