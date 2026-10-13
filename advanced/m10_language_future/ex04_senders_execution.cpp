// m10_language_future/ex04_senders_execution — rasshirennyy model' sender/receiver.
// C++23 (std::execution v GCC 16.2 net); rasshirenie m06_concurrency/ex06.
#include <expected>
#include <format>
#include <iostream>
#include <optional>
#include <print>
#include <string>
#include <utility>

namespace exec {

struct Just {
    int value;
    template <class R>
    struct Op {
        int value;
        R receiver;
        void start() { receiver.set_value(value); }
    };
    template <class R>
    Op<R> connect(R receiver) const { return Op<R>{value, std::move(receiver)}; }
};

inline Just just(int value) { return Just{value}; }

struct JustError {
    std::string error;
    template <class R>
    struct Op {
        std::string error;
        R receiver;
        void start() { receiver.set_error(error); }
    };
    template <class R>
    Op<R> connect(R receiver) const { return Op<R>{error, std::move(receiver)}; }
};

inline JustError just_error(std::string error) { return JustError{std::move(error)}; }

template <class S, class F>
struct Then {
    S sender;
    F f;
    template <class R>
    struct Op {
        S sender;
        F f;
        R receiver;
        struct Inner {
            F f;
            R* out;
            void set_value(int value) { out->set_value(f(value)); }
            void set_error(std::string error) { out->set_error(std::move(error)); }
            void set_stopped() { out->set_stopped(); }
        };
        void start() {
            Inner inner{f, &receiver};
            auto op = sender.connect(std::move(inner));
            op.start();
        }
    };
    template <class R>
    Op<R> connect(R receiver) const { return Op<R>{sender, f, std::move(receiver)}; }
};

template <class S, class F>
Then<S, F> then(S sender, F f) { return Then<S, F>{std::move(sender), std::move(f)}; }

template <class S, class G>
struct UponError {
    S sender;
    G g;
    template <class R>
    struct Op {
        S sender;
        G g;
        R receiver;
        struct Inner {
            G g;
            R* out;
            void set_value(int value) { out->set_value(value); }
            void set_error(std::string error) { out->set_value(g(std::move(error))); }
            void set_stopped() { out->set_stopped(); }
        };
        void start() {
            Inner inner{g, &receiver};
            auto op = sender.connect(std::move(inner));
            op.start();
        }
    };
    template <class R>
    Op<R> connect(R receiver) const { return Op<R>{sender, g, std::move(receiver)}; }
};

template <class S, class G>
UponError<S, G> upon_error(S sender, G g) {
    return UponError<S, G>{std::move(sender), std::move(g)};
}

template <class S1, class S2, class F>
struct WhenAll {
    S1 first;
    S2 second;
    F combine;
    template <class R>
    struct Op {
        struct State {
            int a = 0;
            int b = 0;
            int arrived = 0;
        };
        S1 first;
        S2 second;
        F combine;
        R receiver;
        State state;

        void deliver(std::string error) { receiver.set_error(std::move(error)); }
        void on_value() {
            if (++state.arrived == 2) {
                receiver.set_value(combine(state.a, state.b));
            }
        }
        struct Left {
            Op* op;
            void set_value(int value) { op->state.a = value; op->on_value(); }
            void set_error(std::string error) { op->deliver(std::move(error)); }
            void set_stopped() { op->receiver.set_stopped(); }
        };
        struct Right {
            Op* op;
            void set_value(int value) { op->state.b = value; op->on_value(); }
            void set_error(std::string error) { op->deliver(std::move(error)); }
            void set_stopped() { op->receiver.set_stopped(); }
        };
        void start() {
            auto left = first.connect(Left{this});
            auto right = second.connect(Right{this});
            left.start();
            right.start();
        }
    };
    template <class R>
    Op<R> connect(R receiver) const {
        return Op<R>{first, second, combine, std::move(receiver), typename Op<R>::State{}};
    }
};

template <class S1, class S2, class F>
WhenAll<S1, S2, F> when_all(S1 first, S2 second, F combine) {
    return WhenAll<S1, S2, F>{std::move(first), std::move(second), std::move(combine)};
}

template <class S>
std::expected<int, std::string> sync_wait(S sender) {
    struct R {
        std::optional<int>* value;
        std::optional<std::string>* error;
        void set_value(int v) { *value = v; }
        void set_error(std::string e) { *error = std::move(e); }
        void set_stopped() {}
    };
    std::optional<int> value;
    std::optional<std::string> error;
    auto op = sender.connect(R{&value, &error});
    op.start();
    if (error) {
        return std::unexpected(*error);
    }
    return *value;
}

}  // namespace exec

int main() {
    std::println("mini sender/receiver (rasshirenie m06/ex06):");

    auto good = exec::then(exec::just(41), [](int v) { return v + 1; });
    const auto combined = exec::when_all(good,
                                         exec::upon_error(exec::just_error("boom"),
                                                          [](std::string) { return 100; }),
                                         [](int a, int b) { return a + b; });
    const auto combined_result = exec::sync_wait(combined);
    std::println("  when_all(then(just(41)), upon_error(just_error)) = {}",
                 combined_result ? std::format("{}", *combined_result) : combined_result.error());

    const auto failed = exec::sync_wait(
        exec::then(exec::just_error("fail"), [](int v) { return v; }));
    std::println("  error channel = {}",
                 failed ? std::format("{}", *failed) : failed.error());

    const auto recovered = exec::sync_wait(exec::then(
        exec::upon_error(exec::just_error("x"), [](std::string) { return 5; }),
        [](int v) { return v * 2; }));
    std::println("  upon_error recovery = {}",
                 recovered ? std::format("{}", *recovered) : recovered.error());

    std::println("std::execution availability:");
#if defined(__cpp_lib_senders)
    std::println("  __cpp_lib_senders = {}", static_cast<long>(__cpp_lib_senders));
#else
    std::println("  __cpp_lib_senders: not provided by this library (GCC 16.2)");
#endif

    const bool ok = combined_result.has_value() && *combined_result == 142 &&
                    !failed.has_value() && failed.error() == "fail" &&
                    recovered.has_value() && *recovered == 10;
    std::cout << "senders execution done\n";
    return ok ? 0 : 1;
}
