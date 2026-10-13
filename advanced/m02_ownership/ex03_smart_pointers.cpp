// m02_ownership/ex03_smart_pointers — umnye ukazateli.
// Standart: C++23. Zapusk: ./run.ps1 m02_ownership/ex03_smart_pointers
#include <cstdio>
#include <iostream>
#include <memory>
#include <print>
#include <string>
#include <utility>

namespace {

struct Widget {
    std::string name;
    explicit Widget(std::string n) : name(std::move(n)) {
        std::println("  Widget '{}' created", name);
    }
    ~Widget() { std::println("  Widget '{}' destroyed", name); }
};

std::unique_ptr<Widget> make_widget(std::string name) {
    return std::make_unique<Widget>(std::move(name));
}

}  // namespace

int main() {
    std::println("unique_ptr:");
    {
        auto w = make_widget("unique");
        std::println("  holds '{}'", w->name);
        auto owner = std::move(w);          // peredacha vladeniya
        std::println("  after move, w is null? {}", (w == nullptr));
    }  // owner osvobozhdaet Widget

    std::println("shared_ptr:");
    {
        auto s = std::make_shared<Widget>("shared");
        std::println("  use_count = {}", s.use_count());
        {
            auto s2 = s;
            std::println("  after copy, use_count = {}", s.use_count());
        }
        std::println("  after scope, use_count = {}", s.use_count());
    }

    std::println("weak_ptr:");
    std::weak_ptr<Widget> observer;
    {
        auto owned = std::make_shared<Widget>("observed");
        observer = owned;
        std::println("  expired? {}", observer.expired());
        if (auto locked = observer.lock()) {
            std::println("  locked '{}'", locked->name);
        }
    }
    std::println("  after owner gone, expired? {}", observer.expired());

    std::println("custom deleter (FILE*):");
    const char* path = "ex03_tmp.tmp";
    {
        std::unique_ptr<std::FILE, int (*)(std::FILE*)> file(
            std::fopen(path, "w"), &std::fclose);
        if (file) {
            std::fputs("data", file.get());
            std::println("  wrote to file");
        }
    }  // fclose vyzvan cherez deleter
    std::remove(path);
    std::println("  file closed by deleter");

    std::cout << "smart pointers done\n";
    return 0;
}
