// m01_syntax/ex11_inheritance_polymorphism — nasledovanie i polimorfizm.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex11_inheritance_polymorphism
//
// Primechanie: zdes' namerenno ispol'zuem new/delete, chtoby pokazat'
// neobhodimost' virtual'nogo destruktora. V m02_ownership eto budet
// zameneno na std::unique_ptr.
#include <iostream>
#include <print>
#include <string_view>
#include <vector>

namespace {

constexpr double kPi = 3.14159265358979323846;

struct Shape {
    Shape() = default;
    virtual ~Shape() = default;  // bez virtual -> delete cherez bazovyj ukazatel' = UB

    [[nodiscard]] virtual double area() const = 0;
    [[nodiscard]] virtual std::string_view name() const = 0;
};

struct Circle final : Shape {
    explicit Circle(double radius) : radius_(radius) { ++alive_; }
    ~Circle() override { --alive_; }

    [[nodiscard]] double area() const override { return kPi * radius_ * radius_; }
    [[nodiscard]] std::string_view name() const override { return "circle"; }
    [[nodiscard]] double radius() const { return radius_; }

    static int alive() { return alive_; }

private:
    double radius_{};
    static inline int alive_ = 0;
};

struct Rect final : Shape {
    Rect(double width, double height) : width_(width), height_(height) { ++alive_; }
    ~Rect() override { --alive_; }

    [[nodiscard]] double area() const override { return width_ * height_; }
    [[nodiscard]] std::string_view name() const override { return "rect"; }
    [[nodiscard]] double width() const { return width_; }
    [[nodiscard]] double height() const { return height_; }

    static int alive() { return alive_; }

private:
    double width_{};
    double height_{};
    static inline int alive_ = 0;
};

}  // namespace

int main() {
    std::vector<Shape*> shapes;
    shapes.push_back(new Circle(2.0));
    shapes.push_back(new Rect(3.0, 4.0));
    shapes.push_back(new Circle(1.0));

    double total = 0.0;
    for (const Shape* s : shapes) {
        total += s->area();
        std::println("{} area = {}", s->name(), s->area());
    }
    std::println("total area = {}", total);

    for (const Shape* s : shapes) {
        if (const auto* r = dynamic_cast<const Rect*>(s)) {
            std::println("  rect {}x{}", r->width(), r->height());
        } else if (const auto* c = dynamic_cast<const Circle*>(s)) {
            std::println("  circle r={}", c->radius());
        }
    }

    std::println("alive before delete: circles={} rects={}", Circle::alive(), Rect::alive());
    for (Shape* s : shapes) {
        delete s;  // virtual ~Shape() obespechivaet vyzov ~Circle/~Rect
    }
    shapes.clear();
    std::println("alive after delete:  circles={} rects={}", Circle::alive(), Rect::alive());

    const bool ok = (Circle::alive() == 0) && (Rect::alive() == 0);
    std::cout << "inheritance/polymorphism done\n";
    return total > 0.0 && ok ? 0 : 1;
}
