// m03_templates/ex03_type_traits — type traits i detection idiom.
// Standart: C++23. Zapusk: ./run.ps1 m03_templates/ex03_type_traits
#include <iostream>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

namespace {

// Detection idiom cherez void_t: est' li u T metod size().
template <class T, class = void>
struct has_size : std::false_type {};

template <class T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

// Detection idiom: est' li vlozhennyj tip value_type.
template <class T, class = void>
struct has_value_type : std::false_type {};

template <class T>
struct has_value_type<T, std::void_t<typename T::value_type>>
    : std::true_type {};

// Uslovnaya logika po trait.
template <class T>
std::string size_or_unknown(const T& value) {
    if constexpr (has_size<T>::value) {
        return "size=" + std::to_string(value.size());
    } else {
        return "no size";
    }
}

}  // namespace

int main() {
    static_assert(has_size<std::vector<int>>::value);
    static_assert(!has_size<int>::value);
    static_assert(has_value_type<std::vector<int>>::value);
    static_assert(!has_value_type<int>::value);

    std::println("has_size<vector<int>>  = {}", has_size<std::vector<int>>::value);
    std::println("has_size<int>          = {}", has_size<int>::value);
    std::println("has_value_type<string> = {}", has_value_type<std::string>::value);
    std::println("has_value_type<int>    = {}", has_value_type<int>::value);

    std::println("query<int>:    integral={} pointer={} arithmetic={}",
                 std::is_integral_v<int>, std::is_pointer_v<int>, std::is_arithmetic_v<int>);
    std::println("query<double>: floating={} integral={}",
                 std::is_floating_point_v<double>, std::is_integral_v<double>);
    std::println("query<int*>:   pointer={} same_as<int*>={}",
                 std::is_pointer_v<int*>, std::is_same_v<int*, int*>);

    using NoRef = std::remove_reference_t<int&>;
    using Decayed = std::decay_t<const int&>;
    using Common = std::common_type_t<int, double>;
    std::println("remove_reference_t<int&> is int? {}", std::is_same_v<NoRef, int>);
    std::println("decay_t<const int&> is int?     {}", std::is_same_v<Decayed, int>);
    std::println("common_type_t<int,double> is double? {}",
                 std::is_same_v<Common, double>);

    std::println("size_or_unknown(vector) = {}", size_or_unknown(std::vector<int>{1, 2, 3}));
    std::println("size_or_unknown(string) = {}", size_or_unknown(std::string("hello")));
    std::println("size_or_unknown(int)    = {}", size_or_unknown(42));

    std::cout << "type traits done\n";
    return has_size<std::vector<int>>::value ? 0 : 1;
}
