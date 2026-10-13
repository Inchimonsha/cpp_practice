// th02 — пример: const, constexpr, static.
// C++20. Показывает границы между «известно при компиляции» и «рантайм».

#include <iostream>

namespace {

// Известно при компиляции — можно использовать в массивах, шаблонах, switch.
constexpr int kMaxHp = 100;
constexpr int kTileSize = 32;

// «Рантайм-константа»: имитация значения, которое приходит извне
// (файл конфигурации, аргумент командной строки). Она не меняется
// после инициализации, но и не известна при компиляции.
int read_level_bonus() {
    return 15;
}

// static внутри функции: одна копия на всю программу. После вызова
// сохраняет значение между вызовами.
std::size_t next_spawn_id() {
    static std::size_t id = 0;  // инициализируется один раз
    return ++id;
}

}  // namespace

int clamp_hp(int hp) {
    if (hp < 0) {
        return 0;
    }
    return hp > kMaxHp ? kMaxHp : hp;
}

int main() {
    const int level_bonus = read_level_bonus();  // рантайм, но константа

    std::cout << "kMaxHp   = " << kMaxHp << '\n';
    std::cout << "kTileSize= " << kTileSize << '\n';
    std::cout << "level_bonus (const, рантайм) = " << level_bonus << '\n';

    std::cout << "\nstatic-счётчик:\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "  спавн #" << next_spawn_id() << '\n';
    }

    std::cout << "\nclamp_hp:\n";
    std::cout << "  hp=-40  -> " << clamp_hp(-40) << '\n';
    std::cout << "  hp=70   -> " << clamp_hp(70) << '\n';
    std::cout << "  hp=9999 -> " << clamp_hp(9999) << '\n';
    return 0;
}