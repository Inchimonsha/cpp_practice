// th04 — пример: ссылки, const&, возврат, время жизни. C++20.

#include <iostream>
#include <string>

namespace {

// Мутация «оригинала» через ссылку — «результат наружу».
void apply_bonus(int& value, int bonus) {
    value += bonus;
}

// Чтение без копии: для больших объектов — const&.
std::size_t report(const std::string& name, int value) {
    std::cout << "  " << name << " = " << value << " (имя из " << name.size()
              << " симв.)\n";
    return name.size();
}

struct Player {
    std::string name;
    int hp = 100;
};

// Возврат ссылки НА ОБЪЕКТ С БОЛЬШИМ ВРЕМЕНЕМ ЖИЗНИ — ок (пытаемся не
// менять: поэтому const-версия).
const Player& strongest(const Player& a, const Player& b) {
    return (a.hp >= b.hp) ? a : b;
}

}  // namespace

int main() {
    int score = 10;
    apply_bonus(score, 5);
    std::cout << "score после apply_bonus = " << score << '\n';

    const std::string hero = "Apex";
    report(hero, score);

    Player a{"PlayerA", 120};
    Player b{"PlayerB", 90};
    const Player& best = strongest(a, b);
    std::cout << "сильнейший: " << best.name << " (hp=" << best.hp << ")\n";
    return 0;
}