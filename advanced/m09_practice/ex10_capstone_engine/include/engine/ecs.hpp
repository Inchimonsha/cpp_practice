#pragma once

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <vector>

namespace engine {

using Entity = std::uint32_t;

template <class T>
concept ComponentType = std::is_trivially_copyable_v<T> &&
                        std::is_default_constructible_v<T>;

template <ComponentType... Cs>
class Registry {
    template <class C>
    struct Column {
        std::vector<C> values;
        std::vector<char> present;
    };

public:
    Entity create() {
        const Entity entity = static_cast<Entity>(count_++);
        alive_.push_back(1);
        std::apply([&](auto&... columns) {
            (columns.values.emplace_back(), ...);
            (columns.present.push_back(0), ...);
        }, columns_);
        return entity;
    }

    template <ComponentType C>
    void add(Entity entity, const C& value) {
        auto& column = std::get<Column<C>>(columns_);
        column.values[entity] = value;
        column.present[entity] = 1;
    }

    template <ComponentType C>
    bool has(Entity entity) const {
        return std::get<Column<C>>(columns_).present[entity] != 0;
    }

    template <ComponentType C>
    C& get(Entity entity) {
        return std::get<Column<C>>(columns_).values[entity];
    }

    template <ComponentType C>
    const C& get(Entity entity) const {
        return std::get<Column<C>>(columns_).values[entity];
    }

    bool alive(Entity entity) const {
        return entity < count_ && alive_[entity] != 0;
    }

    void destroy(Entity entity) {
        if (alive(entity)) { alive_[entity] = 0; }
    }

    template <ComponentType... Target, class Func>
    void each(Func&& func) {
        for (Entity entity = 0; entity < count_; ++entity) {
            if (alive_[entity] != 0 && (has<Target>(entity) && ...)) {
                func(entity, get<Target>(entity)...);
            }
        }
    }

    Entity count() const { return count_; }

private:
    std::vector<char> alive_;
    std::tuple<Column<Cs>...> columns_;
    Entity count_ = 0;
};

}  // namespace engine
