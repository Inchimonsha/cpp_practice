// th06 — пример: базовые операции со строками. C++20.

#include <iostream>
#include <sstream>
#include <string>

namespace {

int parse_or_zero(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return 0;
    }
}

}  // namespace

int main() {
    std::string player = "Hero";
    std::string message = "HP 100: " + player;

    std::cout << "конкатенация: " << message << '\n';
    std::cout << "  длина: " << message.size() << '\n';

    const std::size_t pos = message.find("HP");
    std::cout << "  find(\"HP\") = " << pos << '\n';
    std::cout << "  find(\"xyz\") = " << std::string::npos << " (npos)\n";

    if (message.find("100") != std::string::npos) {
        std::cout << "  найдено \"100\"\n";
    }

    std::string score_text = std::to_string(12345);
    std::cout << "  to_string(12345) -> \"" << score_text << "\"\n";
    std::cout << "  stoi(\"42\") -> " << parse_or_zero("42")
              << "; stoi(\"abc\") -> " << parse_or_zero("abc") << '\n';

    // Накопление куска текста без O(n^2) конкатенаций.
    std::ostringstream log;
    for (int i = 0; i < 3; ++i) {
        log << "frame " << i << "; ";
    }
    std::cout << "  ostringstream: " << log.str() << '\n';
    return 0;
}