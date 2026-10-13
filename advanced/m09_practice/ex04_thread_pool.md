# ex04_thread_pool — пул потоков с очередью задач

## Теория

Пул потоков устраняет накладные расходы на создание потока на каждую задачу:

- **Очередь задач** (`std::queue<std::function<void()>>`) защищена мьютексом.
- **Рабочие потоки** (`std::jthread`) ждут на `std::condition_variable` и забирают
  задачи.
- `submit` оборачивает задачу в `std::packaged_task`, кладёт в очередь и отдаёт
  `std::future` — результат, исключение и синхронизацию «в одном флаконе».
- Завершение: выставить флаг `stopping_`, разбудить всех, дождаться завершения
  (RAII `std::jthread` делает join сам).

## Синтаксис

```cpp
class ThreadPool {
public:
    explicit ThreadPool(std::size_t workers = 4);
    ~ThreadPool();
    template <class F>
    auto submit(F&& task) -> std::future<std::invoke_result_t<F>>;
};

auto future = pool.submit([] { return 21 * 2; });
int value = future.get();          // 42
```

## Подводные камни

- `std::future` из `std::packaged_task` **нельзя копировать**; переносите его
  (`std::move`), а задачу кладите в `std::function<void()>` через обёртку.
- Разрушение пула должно сначала остановить рабочих, иначе `std::terminate` при
  join c непустой неотданной задачей. Порядок: `stopping_ = true`, `notify_all`,
  join.
- `notify_all` вызывайте **после** освобождения мьютекса — иначе лишний
  wake-up/contention.
- Задачи, бросившие исключение, не «съедают» поток: ошибка хранится во future и
  извлекается в `get()`.
- `std::thread::hardware_concurrency()` может вернуть `0` — предусмотрите
  fallback.
- Не добавляйте задачи в `submit`, не защитив очередь мьютексом.

## Задача

Реализовать `ThreadPool` с `submit`, возвращающим `std::future`. Отправить 8
задач «квадрат числа» и собрать сумму; 20 задач-инкрементов атомарного счётчика;
одну задачу, бросающую исключение, и перехватить его из future. Вывести суммы,
счётчик и текст ошибки.

## Где применяется

- Серверы, обработка изображений/ассетов, фоновые загрузки, параллельные
  пайплайны сборки/декодирования.

## Проверка

Вывод: сумма квадратов `204`, счётчик `20`, перехваченная ошибка `boom`; код
возврата `0` при совпадении, иначе `1`.

## Решение

```cpp
#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <print>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

namespace {

class ThreadPool {
public:
    explicit ThreadPool(std::size_t workers = 0) {
        if (workers == 0) {
            workers = std::thread::hardware_concurrency();
            if (workers == 0) { workers = 4; }
        }
        for (std::size_t i = 0; i < workers; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard lock(mutex_);
            stopping_ = true;
        }
        cv_.notify_all();
        // std::jthread v workers_ join-itsya pri razrushenii.
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <class F>
    auto submit(F&& task) -> std::future<std::invoke_result_t<F>> {
        using Result = std::invoke_result_t<F>;
        auto packaged = std::make_shared<std::packaged_task<Result()>>(
            std::forward<F>(task));
        std::future<Result> future = packaged->get_future();
        {
            std::lock_guard lock(mutex_);
            tasks_.emplace([packaged] { (*packaged)(); });
        }
        cv_.notify_one();
        return future;
    }

    std::size_t size() const { return workers_.size(); }

private:
    void worker_loop() {
        for (;;) {
            std::function<void()> task;
            {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });
                if (stopping_ && tasks_.empty()) { return; }
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    std::vector<std::jthread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable_any cv_;
    bool stopping_ = false;
};

}  // namespace

int main() {
    ThreadPool pool(4);
    std::println("pool size = {}", pool.size());

    std::vector<std::future<long long>> squares;
    for (int i = 1; i <= 8; ++i) {
        squares.push_back(pool.submit([i] { return static_cast<long long>(i) * i; }));
    }
    long long total = 0;
    for (auto& future : squares) { total += future.get(); }
    std::println("squares sum = {}", total);

    std::atomic<int> counter{0};
    std::vector<std::future<void>> jobs;
    for (int i = 0; i < 20; ++i) {
        jobs.push_back(pool.submit([&counter] {
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }
    for (auto& future : jobs) { future.get(); }
    std::println("tasks executed = {}", counter.load());

    bool caught = false;
    auto bad = pool.submit([]() -> int { throw std::runtime_error("boom"); });
    try {
        (void)bad.get();
    } catch (const std::exception& error) {
        caught = true;
        std::println("caught: {}", error.what());
    }

    std::cout << "thread pool done\n";
    return (total == 204 && counter.load() == 20 && caught) ? 0 : 1;
}
```
