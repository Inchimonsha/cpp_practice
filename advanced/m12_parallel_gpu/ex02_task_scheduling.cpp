// m12_parallel_gpu/ex02_task_scheduling — planirovanie zadach po grafu zavisimostey.
// Standart: C++23. Zapusk: ./run.ps1 m12_parallel_gpu/ex02_task_scheduling
#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <print>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class TaskGraph {
public:
    using Task = std::function<double(const std::vector<double>&)>;

    void add(std::function<double(const std::vector<double>&)> task, std::vector<int> deps) {
        tasks_.push_back(std::move(task));
        dependencies_.push_back(std::move(deps));
    }

    std::vector<double> run(unsigned workers) {
        const std::size_t count = tasks_.size();
        if (count == 0) {
            return {};
        }

        std::vector<std::vector<int>> successors(count);
        std::vector<int> indegree(count, 0);
        for (std::size_t task = 0; task < count; ++task) {
            for (int dependency : dependencies_[task]) {
                successors[static_cast<std::size_t>(dependency)].push_back(static_cast<int>(task));
                ++indegree[task];
            }
        }

        std::vector<double> results(count, 0.0);
        std::queue<int> ready;
        for (std::size_t task = 0; task < count; ++task) {
            if (indegree[task] == 0) {
                ready.push(static_cast<int>(task));
            }
        }

        std::mutex mutex;
        std::condition_variable cv;
        std::size_t completed = 0;

        const auto worker = [&]() {
            while (true) {
                int task = -1;
                {
                    std::unique_lock lock(mutex);
                    cv.wait(lock, [&] { return !ready.empty() || completed == count; });
                    if (ready.empty()) {
                        return;
                    }
                    task = ready.front();
                    ready.pop();
                }

                std::vector<double> inputs;
                inputs.reserve(dependencies_[static_cast<std::size_t>(task)].size());
                for (int dependency : dependencies_[static_cast<std::size_t>(task)]) {
                    inputs.push_back(results[static_cast<std::size_t>(dependency)]);
                }
                const double output = tasks_[static_cast<std::size_t>(task)](inputs);

                {
                    std::lock_guard lock(mutex);
                    results[static_cast<std::size_t>(task)] = output;
                    ++completed;
                    for (int successor : successors[static_cast<std::size_t>(task)]) {
                        if (--indegree[static_cast<std::size_t>(successor)] == 0) {
                            ready.push(successor);
                        }
                    }
                    cv.notify_all();
                }
            }
        };

        const unsigned pool = std::max(1u, std::min<unsigned>(workers, static_cast<unsigned>(count)));
        {
            std::vector<std::jthread> threads;
            for (unsigned index = 0; index < pool; ++index) {
                threads.emplace_back(worker);
            }
        }

        return results;
    }

private:
    std::vector<std::function<double(const std::vector<double>&)>> tasks_;
    std::vector<std::vector<int>> dependencies_;
};

int main() {
    TaskGraph graph;
    graph.add([](const std::vector<double>&) { return 2.0; }, {});
    graph.add([](const std::vector<double>&) { return 3.0; }, {});
    graph.add([](const std::vector<double>& inputs) { return inputs[0] * inputs[1]; }, {0, 1});
    graph.add([](const std::vector<double>& inputs) { return inputs[0] * 10.0; }, {2});
    graph.add([](const std::vector<double>& inputs) { return inputs[0] + inputs[1]; }, {0, 2});

    const std::vector<double> results = graph.run(4);
    for (std::size_t index = 0; index < results.size(); ++index) {
        std::println("task {} = {}", index, results[index]);
    }

    auto base = std::async(std::launch::async, [] { return 21; });
    auto doubled = std::async(std::launch::deferred, [&base] { return 2 * base.get(); });
    const int composition = doubled.get();
    std::println("future composition = {}", composition);

    const bool ok = results.size() == 5 && results[0] == 2.0 && results[1] == 3.0 &&
                    results[2] == 6.0 && results[3] == 60.0 && results[4] == 8.0 &&
                    composition == 42;
    std::cout << "task scheduling done\n";
    return ok ? 0 : 1;
}
