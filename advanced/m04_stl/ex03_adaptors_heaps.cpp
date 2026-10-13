// m04_stl/ex03_adaptors_heaps — adaptory i kuchi.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex03_adaptors_heaps
#include <algorithm>
#include <functional>
#include <iostream>
#include <print>
#include <queue>
#include <stack>
#include <vector>

int main() {
    std::println("stack (LIFO):");
    std::stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    std::print("  pop order:");
    while (!s.empty()) {
        std::print(" {}", s.top());
        s.pop();
    }
    std::print("\n");

    std::println("queue (FIFO):");
    std::queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    std::print("  pop order:");
    while (!q.empty()) {
        std::print(" {}", q.front());
        q.pop();
    }
    std::print("\n");

    std::println("priority_queue max:");
    std::priority_queue<int> maxHeap;
    for (int v : {3, 1, 4, 1, 5}) { maxHeap.push(v); }
    std::print("  pop order:");
    while (!maxHeap.empty()) {
        std::print(" {}", maxHeap.top());
        maxHeap.pop();
    }
    std::print("\n");

    std::println("priority_queue min:");
    std::priority_queue<int, std::vector<int>, std::greater<>> minHeap;
    for (int v : {3, 1, 4, 1, 5}) { minHeap.push(v); }
    std::print("  pop order:");
    while (!minHeap.empty()) {
        std::print(" {}", minHeap.top());
        minHeap.pop();
    }
    std::print("\n");

    std::println("heap algorithms (heapsort):");
    std::vector<int> data{3, 1, 4, 1, 5, 9, 2, 6};
    std::make_heap(data.begin(), data.end());
    std::println("  is_heap = {}", std::is_heap(data.begin(), data.end()));

    data.push_back(7);
    std::push_heap(data.begin(), data.end());

    std::vector<int> sorted;
    while (!data.empty()) {
        std::pop_heap(data.begin(), data.end());   // max -> v konec
        sorted.push_back(data.back());
        data.pop_back();
    }
    std::print("  sorted desc:");
    for (int v : sorted) { std::print(" {}", v); }
    std::print("\n");

    std::cout << "adaptors/heaps done\n";
    return (!sorted.empty() && sorted.front() == 9) ? 0 : 1;
}
