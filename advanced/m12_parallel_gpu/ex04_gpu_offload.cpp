// m12_parallel_gpu/ex04_gpu_offload — model't «offload» na ustroystvo (bez real'nogo GPU).
// Real'nyy GPU-rantaym (CUDA/HIP/OpenCL/SYCL) v srede otsutstvuet, poetomu
// demonstriruetsya arxitekturnaya model': host/device pamyat', zapusk yadra
// setkoy blokov, kopirovanie rezul'tata obratno.
// Standart: C++23. Zapusk: ./run.ps1 m12_parallel_gpu/ex04_gpu_offload
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <print>
#include <thread>
#include <vector>

template <class Kernel>
void launch_kernel(std::size_t n, std::size_t block_size, Kernel kernel) {
    const std::size_t blocks = (n + block_size - 1) / block_size;
    std::vector<std::jthread> workers;
    workers.reserve(blocks);
    for (std::size_t block = 0; block < blocks; ++block) {
        workers.emplace_back([&kernel, block, block_size, n] {
            const std::size_t begin = block * block_size;
            const std::size_t end = std::min(begin + block_size, n);
            for (std::size_t index = begin; index < end; ++index) {
                kernel(index);
            }
        });
    }
}

struct Device {
    std::vector<float> x;
    std::vector<float> y;
};

int main() {
    const std::size_t count = 8;
    const std::size_t block_size = 2;
    const float scale = 2.0f;

    std::vector<float> host_x(count);
    std::vector<float> host_y(count, 0.0f);
    for (std::size_t index = 0; index < count; ++index) {
        host_x[index] = static_cast<float>(index + 1);
    }

    const std::size_t blocks = (count + block_size - 1) / block_size;
    std::println("simulated device: {} elements, {} blocks x {} threads", count, blocks,
                 block_size);

    Device device;
    device.x = host_x;
    device.y = host_y;
    std::println("memcpy host->device: x[{}], y[{}]", device.x.size(), device.y.size());

    std::print("launch axpy kernel: y = {} * x + y\n", scale);
    launch_kernel(count, block_size, [&](std::size_t index) {
        device.y[index] = scale * device.x[index] + device.y[index];
    });

    std::vector<float> result = device.y;
    std::println("memcpy device->host: y[{}]", result.size());

    std::print("result =");
    for (float value : result) {
        std::print(" {}", value);
    }
    std::println("");

    const std::vector<float> expected{2, 4, 6, 8, 10, 12, 14, 16};
    const bool ok = (result == expected);
    std::cout << "gpu offload (simulated) done\n";
    return ok ? 0 : 1;
}
