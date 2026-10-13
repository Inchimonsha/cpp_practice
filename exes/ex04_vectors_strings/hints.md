# Подсказки: ex04 — Event Log

Подсказки открывайте по мере необходимости — сначала попробуйте решить сами.

## Уровень 1. Логика журнала (shared/)

Все функции — однострочные обходы вектора.

```cpp
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
```

## Уровень 2. filter_prefix и average_length

```cpp
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
```

## Уровень 3. Разбор команды в REPL

```cpp
const std::size_t space = line.find(' ');

// Команда: всё до первого пробела (или вся строка, если пробела нет).
std::string_view cmd = std::string_view(line).substr(0, space);

// Аргумент: то, что после пробела. Пуст, если пробела не было.
std::string_view arg = std::string_view(line).substr(space + 1);
if (space == std::string::npos) { arg = std::string_view{}; }  // аккуратно с npos!
```

Безопаснее разделить на две функции:

```cpp
std::string_view cmd_of(const std::string& line) {
    const std::size_t space = line.find(' ');
    const std::size_t command_end = (space == std::string::npos) ? line.size()
                                                                  : space;
    return std::string_view(line).substr(0, command_end);
}

std::string_view arg_of(const std::string& line) {
    const std::size_t space = line.find(' ');
    if (space == std::string::npos) {
        return std::string_view{};
    }
    return std::string_view(line).substr(space + 1);
}
```

Помните: `substr` перенесёт, но не может начинаться позже `size()`, поэтому
аргумент сначала проверяйте на `npos`.

```cpp
if (cmd == "add") { push_event(log, arg); }
else if (cmd == "count") { std::cout << count_matching(log, arg) << '\n'; }
else if (cmd == "filter") { for (auto& e : filter_prefix(log, arg)) { /* печать */ } }
else if (cmd == "stats") { std::cout << log.size() << " " << average_length(log) << '\n'; }
else if (cmd == "quit") { running = false; }
else { /* подсказка */ }
```

## Уровень 4. Если «средняя длина» неверна

- `average_length` пустого журнала должна быть `0.0` (проверка `log.empty()`);
- сумма — `std::size_t`; деление на `double`: `static_cast<double>(total) /
  static_cast<double>(log.size())`.

## Уровень 5. Случаи, которые легко сломать

- `add` без текста: `push_event(log, "")` — ок, пустое событие;
- `filter` с префиксом длиннее событий — `substr(0, prefix.size())` вернёт
  всю строку, сравнение даст `false` — правильно;
- строка `quit` без пробела: `find(' ') == npos` → `arg` пустой.