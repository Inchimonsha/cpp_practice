# ex03_atomics_memory_order — атомики и модель памяти

## Теория

`std::atomic<T>` даёт неделимые операции над `T` без мьютекса. Гарантии
наблюдаемости между потоками описывает модель памяти: у каждой атомарной
операции есть параметр `std::memory_order`.

- `relaxed` — атомарность без упорядочивания относительно других ячеек. Хорош
  для счётчиков, где важен только итог.
- `acquire` (на загрузке) / `release` (на сохранении) — образуют пару
  «публикация/наблюдение»: записи до `release` видны тому, кто выполнил
  `acquire` и прочитал это значение.
- `acq_rel` — и то и другое (для read-modify-write).
- `seq_cst` (по умолчанию) — единый глобальный порядок; самый строгий и самый
  дорогой.

`std::atomic_flag` — простейшая атомарная ячейка; из неё строят спинлок через
`test_and_set`/`clear`. `is_lock_free()` сообщает, используется ли аппаратная
атомарность или скрытый мьютекс.

## Синтаксис

```cpp
std::atomic<int> a{0};
a.fetch_add(1, std::memory_order_relaxed);

std::atomic<bool> ready{false};
ready.store(true, std::memory_order_release);   // publikaciya
while (!ready.load(std::memory_order_acquire)) {} // nablyudenie

std::atomic_flag f = ATOMIC_FLAG_INIT;
while (f.test_and_set(std::memory_order_acquire)) {}
f.clear(std::memory_order_release);
```

## Подводные камни

- `relaxed` **не** устанавливает порядок: если через атомик передаётся флаг
  готовности данных, нужен `release`/`acquire`, иначе чтение данных — гонка.
- `volatile` не заменяет `std::atomic` для межпоточной связи.
- Незащищённые обычные переменные рядом с атомиком всё ещё могут быть гонкой.
- Спинлок на `atomic_flag` активен: долгое удержание съедает CPU; нет
  справедливости, возможен голод.
- Модель памяти сложна: без необходимости используйте `seq_cst`, оптимизируйте
  осознанно.

## Задача

1. 4 потока увеличивают `std::atomic<int>` по 1000 раз; итог 4000.
2. Вывести `is_lock_free()` для `atomic<int>` и `atomic<long long>`.
3. Реализовать спинлок на `std::atomic_flag`, защитить им обычный счётчик: два
   потока по 5000 инкрементов → 10000.
4. Показать обмен через `release`/`acquire`: производитель пишет `data=42` и
   публикует `ready`; потребитель ждёт и читает `42`.

## Где применяется

- Счётчики, флаги остановки, lock-free структуры, публикация настроек/данных.

## Проверка

Вывод: `counter = 4000`, `protected_value = 10000`, `observed = 42`; код
возврата `0` при совпадении, иначе `1`.

## Решение

```cpp
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
```
