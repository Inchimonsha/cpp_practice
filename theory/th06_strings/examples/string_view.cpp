// th06 — пример: std::string_view — чтение без копий и ловушки времени жизни.
// C++20.

#include <iostream>
#include <string>
#include <string_view>

namespace {

// Принимает «окно» в чужие данные, не копируя их.
bool has_prefix(std::string_view s, std::string_view prefix) {
    if (prefix.size() > s.size()) {
        return false;
    }
    return s.substr(0, prefix.size()) == prefix;
}

std::string_view skip_leading_spaces(std::string_view s) {
    std::size_t i = 0;
    while (i < s.size() && s[i] == ' ') {
        ++i;
    }
    return s.substr(i);  // окно в исходные данные
}

}  // namespace

int main() {
    std::string command = "  attack goblin";

    std::cout << "has_prefix:\n";
    std::cout << "  \"attack goblin\" начинается c \"at\": "
              << has_prefix(command, "at") << '\n';
    std::cout << "  с \"ge\": " << has_prefix(command, "ge") << '\n';

    const std::string_view trimmed = skip_leading_spaces(command);
    std::cout << "trimmed: \"" << trimmed << "\" (данные общие с command, "
              << "без копирования)\n";

    // Опасный образец: string_view из временной строки — висячий.
    // std::string_view bad = std::string("temp!").substr(0, 4);
    // std::cout << bad;  // UB!
    std::cout << "(строку с висячим view я комментирую — она UB)\n";
    return 0;
}