// m01_syntax/ex04_control_flow — upravlenie potokom.
// Standart: C++23. Zapusk: ./run.ps1 m01_syntax/ex04_control_flow
#include <iostream>
#include <map>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <vector>

namespace {

std::string fizzbuzz(int n) {
    if (n % 15 == 0) { return "FizzBuzz"; }
    if (n % 3 == 0) { return "Fizz"; }
    if (n % 5 == 0) { return "Buzz"; }
    return std::to_string(n);
}

std::string grade(int score) {
    switch (score / 10) {
        case 10:
        case 9:  return "A";
        case 8:  return "B";
        case 7:  return "C";
        case 6:  return "D";
        default: return "F";
    }
}

// [[fallthrough]]: nakopitel'nyj bonus (namerennoe "provalivanie").
int points_for(char rank) {
    int total = 0;
    switch (rank) {
        case 'S': total += 100; [[fallthrough]];
        case 'A': total += 50;  [[fallthrough]];
        case 'B': total += 20;  break;
        default:  total += 1;   break;
    }
    return total;
}

std::optional<int> find_value(const std::map<std::string, int>& table,
                              std::string_view key) {
    if (const auto it = table.find(std::string(key)); it != table.end()) {
        return it->second;
    }
    return std::nullopt;
}

}  // namespace

int main() {
    std::string line;
    for (int n = 1; n <= 15; ++n) {
        if (!line.empty()) { line += ' '; }
        line += fizzbuzz(n);
    }
    std::println("fizzbuzz: {}", line);

    std::println("grade(95)={} grade(84)={} grade(40)={}", grade(95), grade(84), grade(40));
    std::println("points_for('S')={} points_for('C')={}", points_for('S'), points_for('C'));

    const std::map<std::string, int> scores{{"alice", 10}, {"bob", 7}, {"carol", 3}};
    for (const auto& [name, value] : scores) {
        std::println("  {} -> {}", name, value);
    }

    // if s init-statement: iterator ne utechet v bol'shuyu oblast'.
    if (const auto found = find_value(scores, "bob"); found.has_value()) {
        std::println("bob score = {}", *found);
    }

    // Determinirovannoe "menyu": spisok komand obrabatyvaem do-while.
    const std::vector<std::string> commands{"open", "save", "quit", "ignored"};
    std::size_t index = 0;
    bool running = true;
    do {
        const std::string& cmd = commands[index++];
        std::println("  cmd: {}", cmd);
        if (cmd == "quit") { running = false; }
    } while (running && index < commands.size());

    std::cout << "control flow done\n";
    return 0;
}
