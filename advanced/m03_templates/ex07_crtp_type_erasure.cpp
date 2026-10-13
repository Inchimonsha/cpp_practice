// m03_templates/ex07_crtp_type_erasure — CRTP i stiranie tipov.
// Standart: C++23. Zapusk: ./run.ps1 m03_templates/ex07_crtp_type_erasure
#include <iostream>
#include <memory>
#include <print>
#include <string>
#include <utility>
#include <vector>

namespace {

// CRTP: bazovyj klass znaet tip naslednika.
template <class Derived>
struct Equality {
    friend bool operator!=(const Derived& a, const Derived& b) {
        return !(a == b);
    }
};

struct Point : Equality<Point> {
    int x{};
    int y{};

    Point() = default;
    Point(int xv, int yv) : x(xv), y(yv) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// CRTP-mixin: schetchik ekzemplyarov.
template <class Derived>
struct Counted {
    Counted() { ++created; }
    static inline int created = 0;
};

struct Widget : Counted<Widget> {
    int id{};
    explicit Widget(int i) : id(i) {}
};

// Type erasure: odin interfeys dlya raznyh tipov bez obshchego bazovogo klassa.
class AnyGreeter {
public:
    template <class T>
    explicit AnyGreeter(T obj)
        : self_(std::make_unique<Model<T>>(std::move(obj))) {}

    [[nodiscard]] std::string greet() const { return self_->greet(); }

    AnyGreeter(AnyGreeter&&) noexcept = default;
    AnyGreeter& operator=(AnyGreeter&&) noexcept = default;

private:
    struct Concept {
        virtual ~Concept() = default;
        [[nodiscard]] virtual std::string greet() const = 0;
    };

    template <class T>
    struct Model : Concept {
        T obj_;
        explicit Model(T obj) : obj_(std::move(obj)) {}
        [[nodiscard]] std::string greet() const override { return obj_.greet(); }
    };

    std::unique_ptr<Concept> self_;
};

struct English {
    [[nodiscard]] std::string greet() const { return "Hello"; }
};

struct French {
    [[nodiscard]] std::string greet() const { return "Bonjour"; }
};

struct Japanese {
    [[nodiscard]] std::string greet() const { return "Konnichiwa"; }
};

}  // namespace

int main() {
    const Point a{1, 2};
    const Point b{1, 2};
    const Point c{3, 4};
    std::println("a == b: {}", a == b);
    std::println("a != c: {}", a != c);   // operator!= iz CRTP

    {
        Widget w1(1);
        Widget w2(2);
        (void)w1;
        (void)w2;
        std::println("Widget::created = {}", Widget::created);
    }

    std::vector<AnyGreeter> greeters;
    greeters.emplace_back(English{});
    greeters.emplace_back(French{});
    greeters.emplace_back(Japanese{});

    std::print("greetings:");
    for (const AnyGreeter& g : greeters) {
        std::print(" {}", g.greet());
    }
    std::print("\n");

    std::cout << "crtp/type erasure done\n";
    return (a == b && a != c && Widget::created == 2) ? 0 : 1;
}
