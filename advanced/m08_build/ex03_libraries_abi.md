# ex03_libraries_abi — библиотеки, ABI, стабильность границ

## Теория

**ABI** (application binary interface) — как на уровне машинного кода называются
символы, передаются аргументы, размещаются объекты. Отличия ABI между
компиляторами/версиями ломают совместимость бинарников.

- **Статические** библиотеки (`.a`/`.lib`) встраиваются в исполняемый файл.
- **Динамические** (`.dll`/`.so`) загружаются во время выполнения.
- **Искажение имён** (name mangling) кодирует пространства имён/типы в символ.
  Граница `extern "C"` отключает искажение — так строят стабильные C-API.
- **Видимость**: на ELF (`-fvisibility=hidden`) скрывают внутренние символы,
  экспортируя только публичное. На Windows аналог — `__declspec(dllexport)`.
- **Версионирование ABI**: не менять существующие сигнатуры/раскладку структур;
  меняется раскладка — меняется major-версия.
- **Opaque pointer**: потребитель видит только неполный тип и функции — можно
  менять внутренности без пересборки потребителя.

## Синтаксис

```cpp
// publichnyj C-ABI (name mangling otkl'uchen)
extern "C" {
struct Widget;                       // nepolnyj tip (opaque)
Widget* widget_create(int id);
int  widget_id(const Widget*);
void widget_destroy(Widget*);
}

// potrebitel': RAII poverh C-ABI
using Handle = std::unique_ptr<Widget, decltype(&widget_destroy)>;
Handle h(widget_create(7), &widget_destroy);
```

## Подводные камни

- Разные `libstdc++`/`libc++`/MSVC STL **несовместимы по ABI** для
  `std::string`/`std::vector` и т.п. — не передавайте STL-типы через границу без
  нужды.
- `_GLIBCXX_USE_CXX11_ABI` меняет ABI `std::string`; смешение значений 0/1 при
  линковке — ошибки.
- Изменение раскладки публичной структуры ломает потребителей.
- Забыли освободить ресурсы, выделенные в библиотеке, **тем же аллокатором** —
  падение (см. AGENTS.md про CRT/SFML).
- Скрытие символов без экспорта нужных функций → «undefined reference» у
  потребителя.
- На Windows import/export-макросы (`dllimport`/`dllexport`) обязательны.

## Задача

Смоделировать C-ABI-границу: неполный тип `Widget` и `extern "C"`-функции
`widget_create/widget_id/widget_destroy`. Потребитель владеет ресурсом через
`std::unique_ptr` с кастомным делитером. Вывести id, размер указателя, версию
компилятора/ABI. Код возврата `0`, если id равен `7`.

## Где применяется

- SDK и плагины, поставка бинарников без исходников, интерфейсы между разными
  компиляторами/языками, стабильные C-API (в т.ч. Lua-C API, см. m09).

## Проверка

Вывод: `widget id = 7`, `pointer size = 8 bytes`, строка версии компилятора; код
возврата `0`.

## Решение

```cpp
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

struct Widget {                      // polnoe opredelenie: vnutri biblioteki
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
```
