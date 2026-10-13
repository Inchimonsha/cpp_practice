// build_info.cpp — «проверка окружения».
// C++20. Печатает версию стандарта, компилятор и платформу.
// Используется в мини-практике th01: запуск этого файла доказывает,
// что цепочка «компилятор → линкер → рантайм» работает.

#include <iostream>

// __cplusplus — макрос со значением года стандарта.
// 202002L == C++20, 202100L — превью C++23.
static void print_cpp_standard() {
    std::cout << "C++ стандарт: ";
#if __cplusplus >= 202302L
    std::cout << "C++23+";
#elif __cplusplus >= 202002L
    std::cout << "C++20";
#elif __cplusplus >= 201703L
    std::cout << "C++17";
#else
    std::cout << "старее C++17";
#endif
    std::cout << " (__cplusplus = " << __cplusplus << ")\n";
}

static void print_compiler() {
    std::cout << "Компилятор: ";
#if defined(__clang__)
    std::cout << "Clang " << __clang_major__ << "." << __clang_minor__;
#elif defined(__GNUC__)
    std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined(_MSC_VER)
    std::cout << "MSVC " << _MSC_VER;
#else
    std::cout << "не определён макросами курса";
#endif
    std::cout << '\n';
}

static void print_platform() {
#if defined(_WIN32)
    std::cout << "Платформа: Windows\n";
#elif defined(__APPLE__)
    std::cout << "Платформа: macOS\n";
#elif defined(__linux__)
    std::cout << "Платформа: Linux\n";
#else
    std::cout << "Платформа: другая\n";
#endif
}

int main() {
    print_cpp_standard();
    print_compiler();
    print_platform();
    return 0;
}