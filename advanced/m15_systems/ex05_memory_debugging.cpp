// m15_systems/ex05_memory_debugging — arena-allocator i detektor povrezhdeniya.
// Pokazyvaet: ruchnoy pulevoy allokator, vyravnivanie, canary-guards.
// V real'nosti: ASan/UBSan (net v MinGW), valgrind, guard pages, _GLIBCXX_ASSERTIONS.
// Standart: C++23. Zapusk: ./run.ps1 m15_systems/ex05_memory_debugging
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>
#include <vector>

namespace {

class Arena {
public:
    explicit Arena(std::size_t bytes) : storage_(bytes) {}

    std::byte* allocate(std::size_t size, std::size_t alignment) {
        const std::size_t aligned = (offset_ + alignment - 1) / alignment * alignment;
        if (aligned + size > storage_.size()) {
            return nullptr;
        }
        offset_ = aligned + size;
        return storage_.data() + aligned;
    }

    std::size_t used() const { return offset_; }
    void reset() { offset_ = 0; }

private:
    std::vector<std::byte> storage_;
    std::size_t offset_ = 0;
};

constexpr unsigned char kGuardByte = 0xAB;
constexpr std::size_t kPayload = 8;
constexpr std::size_t kGuard = 8;

}  // namespace

int main() {
    Arena arena(1024);
    std::byte* small = arena.allocate(1, 1);
    std::byte* aligned = arena.allocate(24, 16);
    const bool alignment_ok = (reinterpret_cast<std::uintptr_t>(aligned) % 16) == 0 &&
                              small < aligned;
    std::println("arena used = {}, aligned ptr = {}", arena.used(),
                 alignment_ok ? "yes" : "no");

    std::vector<unsigned char> block(kPayload + kGuard, kGuardByte);
    std::fill_n(block.begin(), kPayload, 0x11);

    const bool intact =
        std::all_of(block.begin() + kPayload, block.end(),
                    [](unsigned char byte) { return byte == kGuardByte; });
    std::println("canary intact = {}", intact ? "true" : "false");

    block.back() = 0x00;
    const bool corruption_detected =
        !std::all_of(block.begin() + kPayload, block.end(),
                     [](unsigned char byte) { return byte == kGuardByte; });
    std::println("corruption detected = {}", corruption_detected ? "true" : "false");

    arena.reset();
    std::println("arena reset = {}", arena.used());

    const bool ok = arena.used() == 0 && alignment_ok && intact && corruption_detected;
    std::cout << "memory debugging done\n";
    return ok ? 0 : 1;
}
