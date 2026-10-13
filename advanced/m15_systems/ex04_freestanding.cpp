// m15_systems/ex04_freestanding — podmnozhestvo yazyka dlya freestanding-sredy.
// V real'nosti: -ffreestanding -fno-exceptions -fno-rtti, bez main/iostream.
// Zdes' demonstriruyutsya te zhe priyomy (MMIO, tablica preryvaniy) v hosted-rezhime.
// Standart: C++23. Zapusk: ./run.ps1 m15_systems/ex04_freestanding
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>

namespace {

constexpr std::size_t kRegStatus = 0;
constexpr std::size_t kRegData = 1;

class Mmio {
public:
    void write(std::size_t index, std::uint32_t value) {
        base()[index] = value;
    }
    std::uint32_t read(std::size_t index) { return base()[index]; }

private:
    std::uint32_t* base() { return registers_.data(); }

    std::array<std::uint32_t, 8> registers_{};
};

std::uint32_t g_timer_count = 0;
std::uint32_t g_keyboard_count = 0;

extern "C" void isr_timer() { ++g_timer_count; }
extern "C" void isr_keyboard() { ++g_keyboard_count; }

using isr_function = void (*)();
const std::array<isr_function, 2> kVectorTable{&isr_timer, &isr_keyboard};

}  // namespace

int main() {
    static_assert(sizeof(std::uint32_t) == 4, "32-bit registr ozhidaetsya");
    static_assert(kVectorTable.size() == 2, "dva preryvaniya");

    Mmio device;
    device.write(kRegData, 0x2A);
    device.write(kRegStatus, 1);

    const std::uint32_t status = device.read(kRegStatus);
    const std::uint32_t data = device.read(kRegData);
    std::println("mmio regs: status = {}, data = 0x{:X}", status, data);

    for (int i = 0; i < 3; ++i) {
        kVectorTable[0]();
    }
    for (int i = 0; i < 2; ++i) {
        kVectorTable[1]();
    }
    std::println("isr timer count = {}, keyboard count = {}", g_timer_count, g_keyboard_count);

    const bool ok = status == 1 && data == 0x2A && g_timer_count == 3 && g_keyboard_count == 2;
    std::cout << "freestanding ok\n";
    return ok ? 0 : 1;
}
