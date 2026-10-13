// th04 — пример: стек и куча. Демонстрация new/delete (только в учебных
// целях!) и правильной замены через std::unique_ptr. C++20.

#include <iostream>
#include <memory>
#include <vector>

namespace {

// Счётчик «живых» экземпляров — чтобы видеть утечки в консоли.
struct Gadget {
    static int alive;
    Gadget() { ++alive; }
    ~Gadget() { --alive; }
};
int Gadget::alive = 0;

}  // namespace

int main() {
    std::cout << "--- Стек и куча ---\n";
    {
        Gadget g_stack;              // на стеке; освободится сам
        std::cout << "живых (стек): " << Gadget::alive << '\n';
    }  // g_stack уничтожен здесь
    std::cout << "живых после блока: " << Gadget::alive << '\n';

    // Учебная демонстрация new/delete — единственное место в курсе.
    // В вашем коде используйте умные указатели (th09).
    {
        Gadget* g_heap = new Gadget();  // куча
        std::cout << "живых (new): " << Gadget::alive << '\n';
        delete g_heap;                  // обязаны вызвать вручную
        std::cout << "живых после delete: " << Gadget::alive << '\n';
    }

    // Правильная замена: std::unique_ptr (детали — в th09).
    {
        std::unique_ptr<Gadget> g_owned = std::make_unique<Gadget>();
        std::cout << "живых (unique_ptr): " << Gadget::alive << '\n';
    }  // удалился сам
    std::cout << "живых после unique_ptr: " << Gadget::alive << '\n';

    // std::vector выделяет буфер в куче, но владеет им сам.
    std::vector<int> big(1000000, 0);
    big[0] = 1;
    std::cout << "std::vector в куче, элементов: " << big.size() << '\n';
    return 0;
}