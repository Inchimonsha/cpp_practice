# ex02_mutex_locks_cv — мьютексы, блокировки, условные переменные

## Теория

Гонка данных (data race) — одновременный доступ к одной ячейке, где хотя бы один
доступ записывающий и нет синхронизации. Это UB, а не «иногда неверный
результат». Базовая защита — взаимное исключение.

- `std::mutex` — простейший мьютекс.
- `std::lock_guard` — RAII-захват на время области видимости.
- `std::unique_lock` — гибкий: можно отложить захват, разблокировать вручную,
  переоткрыть; нужен для `condition_variable`.
- `std::scoped_lock` (C++17) — захватывает **несколько** мьютексов без
  взаимоблокировки (deadlock), используя алгоритм упорядоченного захвата.

`std::condition_variable` позволяет потоку ждать события без активного
ожидания (busy-wait). Ждать **всегда** нужно в цикле под предикатом, иначе
возможны ложные пробуждения (spurious wakeups).

## Синтаксис

```cpp
{ std::lock_guard<std::mutex> g(m); ++counter; }

{ std::scoped_lock lock(mutex_a, mutex_b); /* dva mutexa */ }

std::unique_lock<std::mutex> lock(qm);
cv.wait(lock, [&]{ return !q.empty() || done; });
```

## Подводные камни

- Захват двух мьютексов по одному в разном порядке → deadlock. Используйте
  `std::scoped_lock` или единый порядок.
- `cv.wait()` без предиката просыпается и «просто так» (spurious wakeup) —
  проверяйте условие в цикле/предикате.
- Изменения, которые должен увидеть ожидающий поток, обязаны быть под тем же
  мьютексом, что и `wait`.
- `notify_one()` до `wait()` без предиката теряется; с предикатом —
  безопасно, потому что условие проверяется после пробуждения.
- Не вызывайте `notify_*` под захваченным мьютексом без необходимости — это
  лишнее пробуждение и потеря параллелизма.

## Задача

1. 4 потока под `std::mutex` увеличивают общий счётчик по 1000 раз; итог 4000.
2. Два потока выполняют по 500 операций `std::scoped_lock(a, b)`; итог 1000, без
   deadlock.
3. Производитель кладёт в очередь `std::queue<int>` числа 1..5 и выставляет
   `done`; потребитель под `condition_variable` забирает всё и считает сумму
   (должна быть 15).

## Где применяется

- Совместные структуры данных, пулы задач, паттерн producer/consumer, пул
  соединений, инициализация по требованию.

## Проверка

Вывод: `counter = 4000`, `ops = 1000`, `sum = 15`, код возврата `0` при
совпадении всех трёх, иначе `1`.

## Решение

```cpp
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
```
