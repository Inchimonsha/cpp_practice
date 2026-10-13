# ex01_threads_jthread — потоки и `std::jthread`

## Теория

`std::thread` — базовый поток выполнения. Он **не владеет** ресурсами
автоматически: если объект `std::thread` уничтожить, не вызвав `join()` или
`detach()`, программа аварийно завершится (`std::terminate`). Это создаёт
проблему владения: кому отвечать за остановку потока?

`std::jthread` (C++20) решает её:

- в деструкторе вызывает `request_stop()`, затем `join()` — RAII-владение
  потоком;
- принимает callable с первым параметром `std::stop_token` — кооперативный
  механизм отмены;
- `std::stop_source`/`std::stop_token` позволяют потоку проверять запрос
  остановки через `token.stop_requested()`.

Отмена **кооперативная**: `request_stop()` только выставляет флаг, поток сам
решает, когда завершиться. Принудительно остановить поток в C++ нельзя.

## Синтаксис

```cpp
std::thread t([]{ /* ... */ });
t.join();                       // дождаться

std::jthread jt([](std::stop_token st) {
    while (!st.stop_requested()) { /* rabota */ }
});
jt.request_stop();              // kooperativnaya otmena
// join() v destuktore avtomaticheski
```

## Подводные камни

- Забытый `join()` у `std::thread` → немедленный `std::terminate`.
- `request_stop()` **не** прерывает поток: без проверки токена поток продолжит
  работу.
- Не пишите в `std::cout`/`std::println` из нескольких потоков без
  синхронизации — вывод перемешается.
- Исключение, вылетевшее из потока, не перехватывается в `main`; при выходе из
  callable-обёртки оно вызывает `std::terminate`.
- Число итераций «до остановки» зависит от планировщика — результат
  недетерминирован, поэтому не полагайтесь на него в проверках.

## Задача

Реализовать три демонстрации с детерминированным результатом:

1. `std::thread`: 4 потока увеличивают `std::atomic<int>` по 1000 раз;
   итог обязан равняться 4000.
2. `std::thread`: каждый поток считает свою частичную сумму, по завершении все
   суммы складываются в общий результат.
3. `std::jthread`: рабочий ждёт сигнала через `std::condition_variable_any` с
   `stop_token`; `main` запрашивает остановку — рабочий выставляет флаг
   `cancelled`.

## Где применяется

- Пул воркеров, фоновая загрузка/обработка, сервисные циклы, которые нужно
  корректно завершать при выходе.

## Проверка

Вывод: `total = 4000`, обе частичные суммы дают итог, `cancelled = true`.
Код возврата `0`, иначе `1`.

## Решение

```cpp
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
```
