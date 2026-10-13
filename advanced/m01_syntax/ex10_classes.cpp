// m01_syntax/ex10_classes — klassy.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex10_classes
#include <iostream>
#include <print>
#include <sstream>
#include <string>

namespace {

class Counter {
public:
    Counter() : Counter(0) {}                 // delegiruyushchij konstruktor
    explicit Counter(int start) : value_(start) { ++instances_; }
    Counter(const Counter&) = delete;         // ne kopiruem v etom primere
    Counter& operator=(const Counter&) = delete;
    ~Counter() { --instances_; }

    [[nodiscard]] int value() const { return value_; }

    void tick(int times = 1) {
        value_ += times;
    }

    [[nodiscard]] static int instances() { return instances_; }

    friend std::ostream& operator<<(std::ostream& os, const Counter& c);

private:
    int value_{0};
    static inline int instances_{0};
};

std::ostream& operator<<(std::ostream& os, const Counter& c) {
    return os << "Counter(" << c.value_ << ')';
}

}  // namespace

int main() {
    Counter a;              // 0
    Counter b(5);           // explicit
    a.tick();               // +1
    a.tick(4);              // +4
    b.tick(2);              // +2

    std::println("a = {}, b = {}", static_cast<int>(a.value()), b.value());
    std::println("live counters = {}", Counter::instances());

    std::ostringstream os;
    os << a;
    std::println("ostream: {}", os.str());

    {
        Counter temp(100);
        std::println("inside scope, live = {}", Counter::instances());
        (void)temp;
    }
    std::println("after scope,  live = {}", Counter::instances());

    std::cout << "classes done\n";
    return (a.value() == 5 && b.value() == 7 && Counter::instances() == 2) ? 0 : 1;
}
