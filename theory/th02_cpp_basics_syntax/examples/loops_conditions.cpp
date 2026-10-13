// th02 — пример: условия, switch, циклы, break/continue.
// C++20. Включает мини-практику из раздела 16 теории.

#include <iostream>
#include <vector>

namespace {

// Сумма чётных от 0 до n (включительно). n < 0 — особый случай.
int sum_even(int n) {
    if (n < 0) {
        return -1;
    }
    int total = 0;
    for (int i = 0; i <= n; ++i) {
        if (i % 2 == 0) {
            total += i;
        }
    }
    return total;
}

// Вариант без оператора %: шаг сразу по чётным числам.
int sum_even_step2(int n) {
    if (n < 0) {
        return -1;
    }
    int total = 0;
    for (int i = 0; i <= n; i += 2) {
        total += i;
    }
    return total;
}

const char* pawn_grade(int score) {
    switch (score / 10) {  // только целые/enum
        case 0:
        case 1:
        case 2:
            return "новичок";  // 0..29: проваливание намеренное
        case 3:
        case 4:
            return "опытный";
        case 5:
        case 6:
        case 7:
            return "ветеран";
        default:
            return "легенда";  // 80+
    }
}

}  // namespace

int main() {
    std::cout << "\nsum_even (по модулю):\n";
    std::cout << "  sum_even(10)   = " << sum_even(10) << '\n';
    std::cout << "  sum_even(-3)   = " << sum_even(-3) << "  (особый случай)\n";

    std::cout << "\nsum_even_step2 (без %):\n";
    std::cout << "  sum_even_step2(10) = " << sum_even_step2(10) << '\n';

    std::cout << "\nswitch + циклы:\n";
    const std::vector<int> scores{15, 42, 73, 95};
    for (const int s : scores) {  // range-for
        std::cout << "  score " << s << " -> " << pawn_grade(s) << '\n';
    }

    std::cout << "\nbreak/continue:\n";
    int first_trouble = -1;
    for (int i = 0; i < 10; ++i) {
        if (i % 3 == 0) {
            continue;  // пропустить кратные трём
        }
        if (i > 7) {
            break;  // дальше не идём
        }
        first_trouble = i;
    }
    std::cout << "  последнее обработанное i = " << first_trouble << '\n';

    std::cout << "\nwhile и do-while:\n";
    int hp = 5;
    int turns = 0;
    while (hp > 0) {
        hp -= 2;
        ++turns;
    }
    std::cout << "  while: хитов до смерти = " << turns << '\n';

    int rolls = 0;
    const int target = 6;
    int roll = 0;
    do {
        roll = (roll * 7 + 3) % 10;  // простейший «псевдо-бросок»
        ++rolls;
    } while (roll < target);
    std::cout << "  do-while: бросков до " << target << " = " << rolls << '\n';
    return 0;
}