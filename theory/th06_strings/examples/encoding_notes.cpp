// th06 — пример: байты vs символы в UTF-8. C++20.
// Выводим только ASCII и числа: кириллица в консоли Windows может отобразиться
// неверно из-за кодовой страницы (см. theory.md, раздел 5.3).

#include <iostream>
#include <string>

int main() {
    std::string ascii = "Hello";
    // "Привет" в UTF-8: по 2 байта на символ. Явные байты — чтобы пример не
    // зависел от кодировки исходного файла.
    std::string cyrillic = "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82";

    std::cout << "\"Hello\"  : size = " << ascii.size() << " байт, символов = "
              << ascii.size() << '\n';
    std::cout << "\"Привет\" : size = " << cyrillic.size() << " байт (UTF-8), "
              << "символов = 6\n";

    std::cout << "Разница: в UTF-8 один символ занимает 1-4 байта;\n"
              << "std::string::size() считает БАЙТЫ, а не буквы.\n";
    return 0;
}