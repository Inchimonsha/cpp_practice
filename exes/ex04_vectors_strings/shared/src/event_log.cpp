// event_log.cpp — реализация журнала ex04.

#include <ex04/event_log.hpp>

namespace ex04 {

bool starts_with(std::string_view s, std::string_view prefix) {
    return s.substr(0, prefix.size()) == prefix;
}

void push_event(std::vector<std::string>& log, std::string_view event) {
    log.push_back(std::string(event));
}

std::size_t count_matching(const std::vector<std::string>& log,
                           std::string_view fragment) {
    std::size_t count = 0;
    for (const std::string& event : log) {
        if (event.find(fragment) != std::string::npos) {
            ++count;
        }
    }
    return count;
}

std::vector<std::string> filter_prefix(const std::vector<std::string>& log,
                                       std::string_view prefix) {
    std::vector<std::string> result;
    for (const std::string& event : log) {
        if (starts_with(event, prefix)) {
            result.push_back(event);
        }
    }
    return result;
}

double average_length(const std::vector<std::string>& log) {
    if (log.empty()) {
        return 0.0;
    }
    std::size_t total = 0;
    for (const std::string& event : log) {
        total += event.size();
    }
    return static_cast<double>(total) / static_cast<double>(log.size());
}

}  // namespace ex04