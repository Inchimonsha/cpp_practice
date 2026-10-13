// m08_build/ex03_libraries_abi — biblioteki, ABI, stabil'nost' granic.
// Standart: C++23. Zapusk: ./run.ps1 m08_build/ex03_libraries_abi
#include <cstddef>
#include <iostream>
#include <memory>
#include <print>

namespace {

extern "C" {
struct Widget;                       // nepolnyj tip dlya potrebitelya
Widget* widget_create(int id);
int widget_id(const Widget* widget);
void widget_destroy(Widget* widget);
}

struct Widget {                      // polnoe opredelenie: vnutri "biblioteki"
    int id;
};

Widget* widget_create(int id) { return new Widget{id}; }
int widget_id(const Widget* widget) { return widget != nullptr ? widget->id : -1; }
void widget_destroy(Widget* widget) { delete widget; }

}  // namespace

int main() {
    using Handle = std::unique_ptr<Widget, decltype(&widget_destroy)>;
    Handle handle(widget_create(7), &widget_destroy);

    std::println("widget id = {}", widget_id(handle.get()));
    std::println("pointer size = {} bytes", sizeof(void*));
    std::println("compiler: {}", __VERSION__);
#ifdef __GXX_ABI_VERSION
    std::println("__GXX_ABI_VERSION = {}", __GXX_ABI_VERSION);
#endif

    std::cout << "libraries/abi done\n";
    return widget_id(handle.get()) == 7 ? 0 : 1;
}
