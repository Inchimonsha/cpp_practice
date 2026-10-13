// m10_language_future/ex02_reflection — refleksiya cherez structured bindings.
// C++26 ^^/std::meta v GCC 16.2 net; pokazan rabotayushchiy fallback.
#include <array>
#include <cstddef>
#include <format>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace reflect {

struct any {
    template <class T>
    constexpr operator T() const;
};

template <class T, class... Args>
consteval std::size_t fields_count_impl() {
    if constexpr (requires { T{ Args{}..., any{} }; }) {
        return fields_count_impl<T, Args..., any>();
    } else {
        return sizeof...(Args);
    }
}

template <class T>
consteval std::size_t fields_count() {
    return fields_count_impl<std::remove_cvref_t<T>>();
}

template <class T>
constexpr auto to_tuple(T&& object) {
    using U = std::remove_cvref_t<T>;
    if constexpr (fields_count<U>() == 0) {
        return std::tuple<>{};
    } else if constexpr (fields_count<U>() == 1) {
        auto&& [a] = object;
        return std::forward_as_tuple(a);
    } else if constexpr (fields_count<U>() == 2) {
        auto&& [a, b] = object;
        return std::forward_as_tuple(a, b);
    } else if constexpr (fields_count<U>() == 3) {
        auto&& [a, b, c] = object;
        return std::forward_as_tuple(a, b, c);
    } else {
        auto&& [a, b, c, d] = object;
        return std::forward_as_tuple(a, b, c, d);
    }
}

}  // namespace reflect

struct Point {
    double x;
    double y;
};

struct Player {
    int id;
    std::string name;
    double hp;
};

template <class T>
struct field_names;

template <>
struct field_names<Point> {
    static constexpr std::array<std::string_view, 2> names{"x", "y"};
};

template <>
struct field_names<Player> {
    static constexpr std::array<std::string_view, 3> names{"id", "name", "hp"};
};

template <class V>
std::string format_value(const V& value) {
    if constexpr (std::is_convertible_v<V, std::string_view>) {
        return std::format("\"{}\"", std::string_view(value));
    } else {
        return std::format("{}", value);
    }
}

template <class T, class Tuple, std::size_t... I>
std::string dump_impl(const Tuple& values, std::index_sequence<I...>) {
    std::string out = "{";
    std::size_t written = 0;
    ((out += std::string(written++ ? ", " : "") +
             std::format("\"{}\":{}", field_names<T>::names[I],
                         format_value(std::get<I>(values)))),
     ...);
    return out + "}";
}

template <class T>
std::string dump(const T& object) {
    return dump_impl<T>(reflect::to_tuple(object),
                        std::make_index_sequence<reflect::fields_count<T>()>{});
}

int main() {
#if defined(__cpp_lib_reflection) || defined(__cpp_reflection)
    std::println("mode = std::meta (C++26)");
#else
    std::println("mode = fallback (structured bindings; GCC 16.2 bez ^^/std::meta)");
#endif

    std::println("fields(Point) = {}", reflect::fields_count<Point>());
    std::println("fields(Player) = {}", reflect::fields_count<Player>());

    const Point point{1.0, 2.0};
    const Player player{7, "neo", 100.0};

    const std::string point_text = dump(point);
    const std::string player_text = dump(player);
    std::println("point = {}", point_text);
    std::println("player = {}", player_text);

    const bool ok = reflect::fields_count<Point>() == 2 &&
                    reflect::fields_count<Player>() == 3 &&
                    point_text == R"({"x":1, "y":2})" &&
                    player_text == R"({"id":7, "name":"neo", "hp":100})";
    std::cout << "reflection done\n";
    return ok ? 0 : 1;
}
