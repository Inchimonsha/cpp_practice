// ex04 — Event Log: каркас (starter).
// Заполните TODO. Постановка — в task.md, подсказки — в hints.md.

#include <ex04/event_log.hpp>

#include <iostream>
#include <string>

namespace {

// Читает всю строку (с пробелами) и стирает завершающий '\n'.
std::string read_line() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

}  // namespace

int main() {
    std::vector<std::string> log;
    std::cout << "=== Event Log ===\n"
              << "Команды: add <текст> | count <подстрока> | "
                 "filter <префикс> | stats | quit\n";

    bool running = true;
    while (running) {
        std::cout << "> ";
        const std::string line = read_line();

        // TODO 1: разделить строку на команду и аргумент.
        //   ищем первый пробел: line.find(' ');
        //   левая часть до пробела — команда;
        //   правая часть (или пусто) — аргумент.
        //   Используйте std::string_view на подстроки line.

        // TODO 2: if-else по команде:
        //   add <t>    -> ex04::push_event(log, значение_аргумента)
        //   count <s>  -> вывести ex04::count_matching(log, s)
        //   filter <p> -> вывести каждое событие из ex04::filter_prefix(log, p)
        //   stats      -> вывести ex04::average_length(log) и log.size()
        //   quit       -> running = false
        //   пустая строка / неизвестная команда -> печать подсказки команды
    }
    return 0;
}