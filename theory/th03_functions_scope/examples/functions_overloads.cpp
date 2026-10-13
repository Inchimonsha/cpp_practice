// th03 — пример: перегрузка и способы передачи параметров. C++20.

#include <iostream>
#include <string>

namespace {

// Перегрузка: разные сигнатуры — одно имя.
int need_xp(int level) {  // сколько опыта нужно для следующего уровня
    return level * 100;
}
int need_xp(int level, int curve) {
    return level * curve;
}

// По значению: копия. Для мелких типов — нормально.
int double_hp(int hp) {
    return hp * 2;
}

// По ссылке: меняем оригинал. «Результат наружу».
void heal_full(int& hp, int max_hp) {
    hp = max_hp;
}

// По const&: не копируем большой объект, но и не меняем.
std::size_t describe(const std::string& name) {
    std::cout << "  боец: " << name << ", длина имени = " << name.size() << '\n';
    return name.size();
}

}  // namespace

int main() {
    std::cout << "need_xp:\n";
    std::cout << "  need_xp(5)      = " << need_xp(5) << '\n';
    std::cout << "  need_xp(5, 250) = " << need_xp(5, 250) << '\n';

    int hp = 10;
    std::cout << "\ndouble_hp (по значению):\n";
    std::cout << "  double_hp(hp) = " << double_hp(hp)
              << ", hp остался: " << hp << '\n';

    std::cout << "heal_full (по ссылке):\n";
    std::cout << "  до: hp = " << hp << '\n';
    heal_full(hp, 100);
    std::cout << "  после: hp = " << hp << '\n';

    std::cout << "\ndescribe (по const&):\n";
    const std::string hero = "Коmmaндир";
    const std::size_t len = describe(hero);
    std::cout << "  `hero` не изменён: длина = " << len << '\n';
    return 0;
}