// m09_practice/ex05_event_bus — tipobezopasnaya shina sobytij.
// Standart: C++23. Zapusk: ./run.ps1 m09_practice/ex05_event_bus
#include <any>
#include <cstdint>
#include <functional>
#include <iostream>
#include <print>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

struct KeyPressed { int key; };
struct MouseMoved { int x; int y; };

class EventBus {
public:
    using Token = std::uint64_t;

    template <class E, class F>
    Token subscribe(F&& handler) {
        const Token token = next_token_++;
        handlers_[std::type_index(typeid(E))].push_back(
            {token, [fn = std::forward<F>(handler)](const std::any& payload) {
                 fn(std::any_cast<const E&>(payload));
             }});
        return token;
    }

    template <class E>
    void unsubscribe(Token token) {
        auto it = handlers_.find(std::type_index(typeid(E)));
        if (it == handlers_.end()) { return; }
        auto& list = it->second;
        for (auto slot = list.begin(); slot != list.end(); ++slot) {
            if (slot->id == token) { list.erase(slot); return; }
        }
    }

    template <class E>
    void publish(const E& event) const {
        auto it = handlers_.find(std::type_index(typeid(E)));
        if (it == handlers_.end()) { return; }
        for (const auto& slot : it->second) {
            slot.handler(std::any{event});
        }
    }

private:
    struct Entry {
        Token id;
        std::function<void(const std::any&)> handler;
    };

    std::unordered_map<std::type_index, std::vector<Entry>> handlers_;
    Token next_token_ = 1;
};

}  // namespace

int main() {
    EventBus bus;
    std::vector<std::string> log;

    const auto logger = bus.subscribe<KeyPressed>([&log](const KeyPressed& event) {
        log.push_back("key " + std::to_string(event.key));
        std::println("logger: key {}", event.key);
    });

    int key_count = 0;
    bus.subscribe<KeyPressed>([&key_count](const KeyPressed&) { ++key_count; });

    bus.subscribe<MouseMoved>([](const MouseMoved& event) {
        std::println("mouse: ({}, {})", event.x, event.y);
    });

    bus.publish(KeyPressed{65});
    bus.publish(KeyPressed{66});
    bus.publish(MouseMoved{10, 20});

    bus.unsubscribe<KeyPressed>(logger);
    bus.publish(KeyPressed{67});          // tol'ko schetchik

    std::println("key_count = {}", key_count);

    const bool ok = key_count == 3 && log.size() == 2 &&
                    log[0] == "key 65" && log[1] == "key 66";
    std::cout << "event bus done\n";
    return ok ? 0 : 1;
}
