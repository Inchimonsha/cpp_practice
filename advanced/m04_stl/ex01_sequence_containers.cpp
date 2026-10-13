// m04_stl/ex01_sequence_containers — posledovatel'nye kontejnery.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex01_sequence_containers
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <print>
#include <vector>

namespace {

void demo_vector() {
    std::vector<int> v;
    std::print("vector capacity growth:");
    for (int i = 0; i < 8; ++i) {
        v.push_back(i);
        std::print(" [{}:{}]", v.size(), v.capacity());
    }
    std::print("\n");

    std::vector<int> reserved;
    reserved.reserve(100);
    for (int i = 0; i < 10; ++i) { reserved.push_back(i); }
    std::println("after reserve(100): size={} capacity={}",
                 reserved.size(), reserved.capacity());
}

void demo_deque() {
    std::deque<int> d;
    d.push_front(1);
    d.push_back(2);
    d.push_front(0);
    std::print("deque:");
    for (int x : d) { std::print(" {}", x); }
    std::print("\n");
}

void demo_list() {
    std::list<int> l{5, 3, 1, 4, 2};
    l.sort();
    l.remove_if([](int x) { return x % 2 == 0; });
    std::print("list sorted+odd:");
    for (int x : l) { std::print(" {}", x); }
    std::print("\n");
}

void demo_forward_list() {
    std::forward_list<int> f{2, 3, 4};
    f.push_front(1);                              // 1 2 3 4
    f.insert_after(f.begin(), 0);                 // 1 0 2 3 4
    f.erase_after(f.before_begin());              // udalit' pervoe (1)
    std::print("forward_list:");
    for (int x : f) { std::print(" {}", x); }
    std::print("\n");
}

}  // namespace

int main() {
    demo_vector();
    demo_deque();
    demo_list();
    demo_forward_list();

    std::cout << "sequence containers done\n";
    return 0;
}
