# ex06_execution_senders — модель sender/receiver (`std::execution`)

## Теория

`std::execution` (P2300, принят в C++26) описывает **асинхронность как
значения**. Вместо того чтобы «где-то запускать поток», вы строите описание
вычисления (sender) и затем подключаете к нему получателя (receiver).

- **sender** — описание отложенного вычисления, которое что-то производит;
- **receiver** — набор обработчиков завершения: `set_value`, `set_error`,
  `set_stopped`;
- `connect(sender, receiver)` создаёт **operation state** — конкретное
  исполнение;
- `start(op)` запускает его (обычно неблокирующе);
- **scheduler** задаёт, где/когда выполняется работа (`sched.schedule()` даёт
  sender);
- операторы-адаптеры: `then`, `let_value`, `when_all`, `sync_wait`, `bulk` и
  конвейерный оператор `|`.

Ключевая идея: отмена, ошибки и переключение контекста — часть типа, а не
внешняя дисциплина. Это даёт композицию и статическую проверку.

## Синтаксис (целевой, C++26)

```cpp
auto work = ex::just(42)
          | ex::then([](int v) { return v + 1; })
          | ex::then([](int v) { return v * 2; });

int result = ex::sync_wait(work).value();   // 86

auto on_pool = ex::schedule(thread_pool.scheduler())
             | ex::then([]{ return compute(); });
```

## Подводные камни

- На 2026-й год не все библиотеки реализуют `<execution>`: GCC 16.2 её **не
  предоставляет** (`__cpp_lib_senders` не определён). Код держите под
  feature-тестом.
- sender-выражение **ничего не делает**, пока не вызваны `connect`+`start`
  либо `sync_wait`/`start_detached`.
- Куда попадёт работа, решает scheduler; без него — на «текущем» контексте.
- receiver должен быть вызван **ровно один** из `set_*`; иначе нарушение
  контракта (terminate/UB).
- Отмена — кооперативная: `set_stopped` приходит из stop-token.

## Задача

Так как стандартная `<execution>` недоступна, **своими руками** смоделировать
модель: минимальные `just(value)` и `then(sender, f)` (синхронные), `connect`
через operation state и `start()`, а также receiver со `set_value`/`set_error`/
`set_stopped`. Собрать конвейер `just(42) → then(+1)` и получить 43. Дополнительно
вывести доступность `__cpp_lib_senders`.

## Где применяется

- Асинхронный ввод-вывод, пулы потоков, GPU/тайловые планировщики, сети: везде,
  где нужна композиция асинхронных шагов, отмена и переключение контекста.

## Проверка

Вывод: `composed value = 43`, отчёт о `__cpp_lib_senders`; код возврата `0` при
корректной композиции, иначе `1`.

## Решение

```cpp
#include <exception>
#include <iostream>
#include <print>
#include <utility>
#include <version>

namespace mini {

template <class Receiver>
struct JustOp {
    int value;
    Receiver receiver;
    void start() { receiver.set_value(value); }
};

struct JustSender {
    int value;
    template <class Receiver>
    JustOp<Receiver> connect(Receiver receiver) const {
        return JustOp<Receiver>{value, std::move(receiver)};
    }
};

inline JustSender just(int value) { return JustSender{value}; }

template <class Sender, class F>
struct ThenSender {
    Sender sender;
    F f;

    template <class Receiver>
    struct Op {
        Sender sender;
        F f;
        Receiver receiver;

        struct Inner {
            F f;
            Receiver* out;
            void set_value(int value) { out->set_value(f(value)); }
            void set_error(std::exception_ptr err) { out->set_error(err); }
            void set_stopped() { out->set_stopped(); }
        };

        void start() {
            Inner inner{f, &receiver};
            auto op = sender.connect(std::move(inner));
            op.start();
        }
    };

    template <class Receiver>
    Op<Receiver> connect(Receiver receiver) const {
        return Op<Receiver>{sender, f, std::move(receiver)};
    }
};

template <class Sender, class F>
ThenSender<Sender, F> then(Sender sender, F f) {
    return ThenSender<Sender, F>{std::move(sender), std::move(f)};
}

struct Sink {
    int* slot;
    void set_value(int value) const { *slot = value; }
    void set_error(std::exception_ptr) const {}
    void set_stopped() const {}
};

}  // namespace mini

int main() {
    std::println("mini sender/receiver model:");
    int got = 0;
    auto pipeline = mini::then(mini::just(42), [](int v) { return v + 1; });
    auto op = pipeline.connect(mini::Sink{&got});
    op.start();
    std::println("  composed value = {}", got);

    std::println("std::execution availability:");
#if defined(__cpp_lib_senders)
    std::println("  __cpp_lib_senders = {}", static_cast<long>(__cpp_lib_senders));
#else
    std::println("  __cpp_lib_senders: not provided by this library (GCC 16.2)");
#endif

    std::cout << "execution senders done\n";
    return got == 43 ? 0 : 1;
}
```
