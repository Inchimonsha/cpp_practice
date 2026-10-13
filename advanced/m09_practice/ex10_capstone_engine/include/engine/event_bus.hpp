#pragma once

#include <any>
#include <cstdint>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace engine {

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

}  // namespace engine
