// ex04 — Event Log: эталонное решение. C++20.
// Логика журнала — в shared/ (event_log.hpp); здесь только REPL.

#include <ex04/event_log.hpp>

#include <iostream>
#include <string>
#include <string_view>

namespace {

std::string read_line() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void print_help() {
    std::cout << "Команды: add <текст> | count <подстрока> | "
                 "filter <префикс> | stats | quit\n";
}

std::string_view command_of(const std::string& line) {
    const std::size_t space = line.find(' ');
    if (space == std::string::npos) {
        return std::string_view(line);
    }
    return std::string_view(line).substr(0, space);
}

std::string_view argument_of(const std::string& line) {
    const std::size_t space = line.find(' ');
    if (space == std::string::npos) {
        return std::string_view{};
    }
    std::string_view rest(line.data() + space + 1, line.size() - space - 1);
    return rest;
}

}  // namespace

int main() {
    std::vector<std::string> log;
    std::cout << "=== Event Log ===\n";
    print_help();

    bool running = true;
    while (running) {
        std::cout << "> ";
        const std::string line = read_line();

        if (line.empty()) {
            print_help();
            continue;
        }

        const std::string_view cmd = command_of(line);
        const std::string_view arg = argument_of(line);

        if (cmd == "add") {
            ex04::push_event(log, arg);
        } else if (cmd == "count") {
            std::cout << ex04::count_matching(log, arg) << '\n';
        } else if (cmd == "filter") {
            const auto events = ex04::filter_prefix(log, arg);
            for (const std::string& event : events) {
                std::cout << event << '\n';
            }
        } else if (cmd == "stats") {
            std::cout << "событий: " << log.size()
                      << ", средняя длина: " << ex04::average_length(log)
                      << '\n';
        } else if (cmd == "quit") {
            running = false;
        } else {
            std::cout << "Неизвестная команда.\n";
            print_help();
        }
    }
    return 0;
}