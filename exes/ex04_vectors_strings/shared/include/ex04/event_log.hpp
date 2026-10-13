// event_log.hpp — логика журнала ex04. Контракт для solution и tests.
// Никакой консоли и случайности — функции «чистые».

#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace ex04 {

// Начинается ли s с prefix (пустой prefix → true).
bool starts_with(std::string_view s, std::string_view prefix);

// Добавляет событие в конец журнала.
void push_event(std::vector<std::string>& log, std::string_view event);

// Сколько событий журнала содержат fragment (с учётом регистра).
std::size_t count_matching(const std::vector<std::string>& log,
                           std::string_view fragment);

// Новый журнал из событий, начинающихся с prefix (порядок сохраняется).
std::vector<std::string> filter_prefix(const std::vector<std::string>& log,
                                       std::string_view prefix);

// Средняя длина строк журнала (0.0 для пустого журнала).
double average_length(const std::vector<std::string>& log);

}  // namespace ex04