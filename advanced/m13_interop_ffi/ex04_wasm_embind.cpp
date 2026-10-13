// m13_interop_ffi/ex04_wasm_embind — model' WASM/embind (bez Emscripten v srede).
// Demonstriruyutsya granica JS<->WASM, lineynaya pamyat' i eksporty; real'naya
// sborka trebuet emcc (otsutstvuet), poetomu ispol'zuetsya ekvivalentnaya model'.
// Standart: C++23. Zapusk: ./run.ps1 m13_interop_ffi/ex04_wasm_embind
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <print>
#include <span>
#include <string>
#include <utility>
#include <vector>

class LinearMemory {
public:
    explicit LinearMemory(std::size_t words) : data_(words, 0) {}

    void store(std::size_t offset, std::span<const std::int32_t> values) {
        for (std::size_t index = 0; index < values.size(); ++index) {
            data_[offset + index] = values[index];
        }
    }

    std::int32_t load(std::size_t offset) const { return data_[offset]; }

    std::size_t size() const { return data_.size(); }

private:
    std::vector<std::int32_t> data_;
};

class WasmModule {
public:
    using Export = std::function<std::int32_t(std::span<const std::int32_t>)>;

    void expose(const std::string& name, Export function) {
        exports_.insert_or_assign(name, std::move(function));
    }

    std::int32_t call(const std::string& name, std::span<const std::int32_t> arguments) const {
        return exports_.at(name)(arguments);
    }

    LinearMemory& memory() { return memory_; }

private:
    std::map<std::string, std::function<std::int32_t(std::span<const std::int32_t>)>> exports_;
    LinearMemory memory_{16};
};

int main() {
    WasmModule module;
    module.expose("add", [](std::span<const std::int32_t> arguments) {
        return arguments[0] + arguments[1];
    });
    module.expose("sum_memory", [&module](std::span<const std::int32_t> arguments) {
        std::int32_t total = 0;
        for (std::int32_t index = 0; index < arguments[1]; ++index) {
            total += module.memory().load(static_cast<std::size_t>(arguments[0] + index));
        }
        return total;
    });

    const std::array<std::int32_t, 4> values{1, 2, 3, 4};
    module.memory().store(0, values);

    const std::array<std::int32_t, 2> add_arguments{2, 3};
    const std::array<std::int32_t, 2> sum_arguments{0, 4};

    std::println("wasm (simulated): exposed add, sum_memory");
    std::println("js: add(2, 3) = {}", module.call("add", add_arguments));
    std::println("js: sum_memory(0, 4) = {}", module.call("sum_memory", sum_arguments));

    const bool ok = module.call("add", add_arguments) == 5 &&
                    module.call("sum_memory", sum_arguments) == 10;
    std::cout << "wasm embind (simulated) done\n";
    return ok ? 0 : 1;
}
