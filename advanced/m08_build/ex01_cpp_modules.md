# ex01_cpp_modules — модули C++ (`import`/`export`)

## Теория

Модули (C++20) заменяют текстовую `#include`-модель на настоящую систему
компоновки:

- **Модульный интерфейсный юнит** объявляется как `export module имя;` и
  экспортирует декларации через `export`.
- Потребитель пишет `import имя;` вместо `#include`; препроцессор в это не
  вмешивается.
- Модуль компилируется **один раз** в бинарный интерфейс (BMI/gcm.cache), что
  ускоряет сборку и устраняет макрос-протечки.
- Порядок инициализации, ODR и изоляция становятся строже; макросы модуля не
  видны импортёру.

В GCC модули включаются флагом `-fmodules-ts` (TS = technical specification).
GCC 16.2 собирает и `export module`, и `import`. `import std;` (стандартная
библиотека как модуль) требует предварительно собранного `std` и здесь не
используется.

## Синтаксис

```cpp
// math.cppm
export module math;
namespace math {
export int add(int a, int b) { return a + b; }
}

// main.cpp
#include <print>
import math;
int main() { std::println("{}", math::add(2, 3)); }
```

```text
g++ -std=c++23 -fmodules-ts math.cppm main.cpp -o app -lstdc++exp
```

## Подводные камни

- Нужен `-fmodules-ts`; иначе `import` не распознаётся.
- Расширение `.cppm`/`.ixx` — соглашение; важен флаг, а не имя.
- Кэш модулей (`gcm.cache/`) должен быть доступен импортёру; при «чистой»
  сборке он создаётся заново.
- `import std;` и модульность стандартной библиотеки в GCC пока требуют
  отдельной настройки — здесь используется обычный `#include`.
- CMake-поддержка модулей требует CMake ≥ 3.28 и `CXX_MODULES`-наборов файлов;
  версии линеек компиляторов различаются.

## Задача

Создать модуль `math` (функции `add`, `mul`) и потребителя `main`, который
печатает `3+4=7` и `3*4=12` с корректным кодом возврата. Файлы лежат в каталоге
задания; сборка — скриптом `build.cmd`.

## Где применяется

- Крупные кодовые базы: ускорение инкрементальной сборки, изоляция API, отказ от
  include-магии, чистые границы компонентов.

## Проверка

Сборка `build.cmd` даёт исполняемый файл; вывод `3+4=7`, `3*4=12`; код возврата
`0`. `__cpp_modules` определён GCC 16.2.

## Решение

`math.cppm`:

```cpp
export module math;

namespace math {
export int add(int a, int b) { return a + b; }
export int mul(int a, int b) { return a * b; }
}  // namespace math
```

`main.cpp`:

```cpp
#include <print>
import math;

int main() {
    std::println("3+4={}", math::add(3, 4));
    std::println("3*4={}", math::mul(3, 4));
    return (math::add(3, 4) == 7 && math::mul(3, 4) == 12) ? 0 : 1;
}
```

`build.cmd`:

```bat
@echo off
setlocal
set PATH=C:\msys64\ucrt64\bin;%PATH%
g++ -std=c++23 -fmodules-ts math.cppm main.cpp -o app.exe -lstdc++exp
app.exe
echo exit=%ERRORLEVEL%
```
