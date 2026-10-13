// m02_ownership/ex02_special_members — pravilo 0/3/5.
// Standart: C++23. Zapusk: ./run.ps1 m02_ownership/ex02_special_members
#include <cstddef>
#include <iostream>
#include <print>
#include <utility>
#include <vector>

namespace {

class Buffer {
public:
    Buffer() = default;

    explicit Buffer(std::size_t n)
        : size_(n), data_(n ? new int[n]{} : nullptr) {
        std::println("  ctor(n={})", n);
    }

    ~Buffer() {
        delete[] data_;
        std::println("  dtor(size={})", size_);
    }

    // Kopiruyushchij konstruktor (glubokaya kopiya).
    Buffer(const Buffer& other)
        : size_(other.size_), data_(other.size_ ? new int[other.size_] : nullptr) {
        for (std::size_t i = 0; i < size_; ++i) { data_[i] = other.data_[i]; }
        std::println("  copy-ctor(size={})", size_);
    }

    // Copy-swap: odna funkciya dlya copy- i move-prisvaivaniya.
    Buffer& operator=(Buffer other) noexcept {
        swap(other);
        std::println("  operator=(size={})", size_);
        return *this;
    }

    // Peremeshchayushchij konstruktor.
    Buffer(Buffer&& other) noexcept
        : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;
        std::println("  move-ctor(size={})", size_);
    }

    void swap(Buffer& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    void set(std::size_t index, int value) { data_[index] = value; }
    [[nodiscard]] int at(std::size_t index) const { return data_[index]; }
    [[nodiscard]] std::size_t size() const { return size_; }

private:
    std::size_t size_{};
    int* data_{};
};

}  // namespace

int main() {
    Buffer original(3);
    original.set(0, 10);
    original.set(1, 20);
    original.set(2, 30);

    std::println("copy:");
    Buffer copy = original;                       // copy-ctor
    std::println("  copy.at(1) = {}", copy.at(1));

    std::println("move:");
    Buffer moved = std::move(original);           // move-ctor
    std::println("  moved.size() = {}, original.size() = {}",
                 moved.size(), original.size());

    std::println("self-assignment (copy-swap):");
    moved = moved;                                // bezopasno
    std::println("  moved.at(2) = {}", moved.at(2));

    std::println("vector growth moves:");
    std::vector<Buffer> buffers;
    buffers.push_back(Buffer(1));
    buffers.push_back(Buffer(2));
    buffers.push_back(Buffer(3));

    std::cout << "special members done\n";
    return (copy.at(1) == 20 && moved.at(2) == 30 && buffers.size() == 3) ? 0 : 1;
}
