// m06_concurrency/ex06_execution_senders — model' sender/receiver (std::execution).
// Standart: C++23. Zapusk: ./run.ps1 m06_concurrency/ex06_execution_senders
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
