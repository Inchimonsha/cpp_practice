# Cheatsheet: th06 — строки

## std::string — владелец

```cpp
std::string s = "abc";
s += 'd';                    // добавить символ
s.append("ef");              // добавить строку
s.size();                    // длина в байтах (в UTF-8 — не «буквы»!)
s.substr(pos, len);          // КОПИЯ
s.find("d");                 // pos или std::string::npos
s.c_str();                   // const char* (с '\0')
std::to_string(42);          // "42"
int v = std::stoi("42");     // 42; ошибка → исключение
```

## std::string_view — чтение без копии (C++17)

```cpp
std::string_view sv = "literal";          // окно на литерал
std::string_view part(s.data() + 1, 3);   // окно в середину строки
bool found = sv.find("ab") != std::string_view::npos;
void f(std::string_view text);            // параметр «прочитать текст»

// ОПАСНО: временная строка!
std::string_view bug = std::string("x").substr(0, 1);  // висячий view
```

## Ловушки

```cpp
if (s.find("x") != std::string::npos) { }   // npos, не -1
s == "abc";   // ок (но "1" + s — нет)
std::stoi("abc");  // throw std::invalid_argument
s.size();     // байты! "А" в UTF-8 занимает 2 байта
```

## Правила курса

- Параметр «прочитать текст» — `std::string_view`; «хранить текст» — `std::string`.
- В логи/ключевые строки — ASCII (консоль Windows).
- Конкатенации в цикле — `append`/`ostringstream`, не `s = s + x;`.