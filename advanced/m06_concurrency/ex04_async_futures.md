# ex04_async_futures — `std::async`, `future`, `promise`, `packaged_task`

## Теория

Асинхронность «результат когда-нибудь» моделируется через `std::future<T>` —
объект, из которого результат забирают методом `get()` (при необходимости
блокируясь). Производитель результата бывает трёх видов:

- `std::async(policy, f)` — запускает `f` и возвращает `future`. Политика
  `std::launch::async` заставляет новый поток, `std::launch::deferred` —
  отложенное выполнение в момент `get()`/`wait()` (в вызывающем потоке),
  комбинация или отсутствие — на усмотрение реализации.
- `std::promise<T>` — канал ручного производства: `set_value`/`set_exception`.
- `std::packaged_task<R(Args...)>` — обёртка callable, чей результат попадает в
  `future`; удобна для очередей задач и пулов потоков.

`get()` можно вызвать только один раз: `future` — move-only и после `get()`
становится недействительным. Исключения из задачи пробрасываются наружу через
`get()`.

## Синтаксис

```cpp
auto f = std::async(std::launch::async, []{ return 42; });
int v = f.get();                       // blokiruetsya do gotovnosti

std::promise<int> p;
std::future<int> fut = p.get_future();
std::thread{[&]{ p.set_value(7); }}.detach();
int x = fut.get();

std::packaged_task<int(int)> task([](int n){ return n * n; });
auto tf = task.get_future();
std::thread t(std::move(task), 5);
t.join();
int sq = tf.get();
```

## Подводные камни

- `std::async` без явной политики может выполниться **синхронно** (deferred) —
  не полагайтесь на параллелизм.
- `std::future` из `std::async`: деструктор может **блокироваться** до
  завершения задачи — забудете `get()`, получите неожиданную точку ожидания.
- `get()` дважды → исключение `std::future_error`.
- `packaged_task` move-only: передавайте `std::move`.
- Не забывайте `join()`/`detach()` у потоков, работающих с `promise`, иначе
  возможен `std::terminate` или потеря исключения.

## Задача

Показать четыре способа получить асинхронный результат и напечатать значения:

1. `std::async(std::launch::async, ...)` → `6*7 = 42`.
-  `std::promise`/`future`: другой поток кладёт `99`.
- `std::packaged_task`: квадрат числа 7 → 49.
- `std::async(std::launch::deferred, ...)`: тело выполняется в момент `get()`.

## Где применяется

- Параллельные вычисления, фоновая загрузка, пул потоков, запросы с таймаутом
  (`wait_for`/`wait_until`), распространение исключений между потоками.

## Проверка

Вывод: `result = 42`, `value = 99`, `square = 49`, тело deferred выполняет
строка до печати результата; код возврата `0`, если все три значения верны.

## Решение

```cpp
#include <future>
#include <iostream>
#include <print>
#include <thread>

int main() {
    std::println("std::async:");
    auto f = std::async(std::launch::async, [] { return 6 * 7; });
    std::println("  result = {}", f.get());

    std::println("promise/future:");
    std::promise<int> p;
    auto fut = p.get_future();
    std::thread t([&p] { p.set_value(99); });
    t.join();
    std::println("  value = {}", fut.get());

    std::println("packaged_task:");
    std::packaged_task<int(int)> task([](int x) { return x * x; });
    auto tf = task.get_future();
    std::thread t2(std::move(task), 7);
    t2.join();
    std::println("  square = {}", tf.get());

    std::println("deferred:");
    auto d = std::async(std::launch::deferred, [] {
        std::println("  (deferred body runs now)");
        return 5;
    });
    std::println("  deferred result = {}", d.get());

    std::cout << "async/futures done\n";
    return 0;
}
```
