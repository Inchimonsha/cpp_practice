// m02_ownership/ex04_move_semantics — move-semantika.
// Standart: C++23. Zapusk: ./run.ps1 m02_ownership/ex04_move_semantics
#include <iostream>
#include <print>
#include <string>
#include <utility>
#include <vector>

namespace {

class Tracked {
public:
    explicit Tracked(int id) : id_(id) { std::println("  ctor(id={})", id_); }
    ~Tracked() { std::println("  dtor(id={})", id_); }

    Tracked(const Tracked&) = delete;
    Tracked& operator=(const Tracked&) = delete;

    Tracked(Tracked&& other) noexcept : id_(std::exchange(other.id_, -1)) {
        std::println("  move-ctor -> id={}", id_);
    }
    Tracked& operator=(Tracked&& other) noexcept {
        id_ = std::exchange(other.id_, -1);
        std::println("  move-assign -> id={}", id_);
        return *this;
    }

    [[nodiscard]] int id() const { return id_; }

private:
    int id_;
};

}  // namespace

int main() {
    std::println("move-ctor:");
    Tracked a(1);
    Tracked b = std::move(a);                 // move-ctor
    std::println("  a.id = {} (moved-from), b.id = {}", a.id(), b.id());

    std::println("move-assign:");
    Tracked c(2);
    c = std::move(b);                         // move-assign
    std::println("  b.id = {} (moved-from), c.id = {}", b.id(), c.id());

    std::println("std::string move:");
    std::string source = "payload";
    std::string target = std::move(source);
    std::println("  source.empty() = {}, target = '{}'", source.empty(), target);
    source = "reused";                        // moved-from mozhno pereinit'.
    std::println("  source after reuse = '{}'", source);

    std::println("vector<string> growth moves:");
    std::vector<std::string> parts;
    for (int i = 0; i < 3; ++i) {
        parts.push_back(std::string(100, static_cast<char>('a' + i)));
    }
    std::println("  parts.size() = {}, parts[0][0] = '{}'", parts.size(), parts[0][0]);

    std::cout << "move semantics done\n";
    return c.id() == 1 ? 0 : 1;
}
