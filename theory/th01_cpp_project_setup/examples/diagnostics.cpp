// diagnostics.cpp — разница Debug/Release.
// C++20.
//
// В Debug assert сработает (program abort), если проверка не выполняется;
// в Release (задан NDEBUG) assert компилируется в пустоту — отсюда и
// «пропажа» проверок. Запускайте обе версии:
//   Debug:   g++ -std=c++20 -O0 -g diagnostics.cpp
//   Release: g++ -std=c++20 -O2 -DNDEBUG diagnostics.cpp

#include <cassert>
#include <iostream>

int main() {
#ifdef NDEBUG
    std::cout << "Сборка: Release (NDEBUG задан)" << '\n';
#else
    std::cout << "Сборка: Debug (NDEBUG не задан)" << '\n';
#endif

    const int health = 100;
    // assert — «вратарь» инвариантов в отладке, но не средство доставки логики.
    assert(health >= 0 && "health не может быть отрицательным");

    if (health < 0) {
        // Корректная обработка для обоих типов сборки.
        std::cerr << "Ошибка: отрицательное здоровье!" << '\n';
        return 1;
    }

    std::cout << "health = " << health << '\n';
    return 0;
}