# m13_interop_ffi/ex01_c_abi — C-ABI: `extern "C"`, указатели на функции, opaque handle

> Модуль m13_interop_ffi · Сложность ★★ · C++23 · Запуск: `./run.ps1 m13_interop_ffi/ex01_c_abi`

## Теория

Граница между языками/библиотеками проходит по **C ABI** — стабильному
двоичному интерфейсу C. Ключевые правила:

- **`extern "C"` снимает C++ name mangling**: имя функции не украшается типами,
  поэтому её найдёт C, Python (`ctypes`), Rust (`extern "C"`), Go (cgo).
- **Только C-совместимые типы**: целые, указатели, POD-структуры
  (standard-layout, trivially copyable). Классы с vtables/исключениями —
  нельзя.
- **Нет перегрузки**: имена `extern "C"` должны быть уникальны.
- **Opaque handle**: C не знает структуру — работает через указатель
  `Counter*`; создание/уничтожение выполняет библиотека (`create`/`destroy`).
  Это защищает ABI от изменения внутреннего представления.

Указатель на функцию C-ABI (`int(*)(int,int)`) — основа callback-ов и таблиц
виртуальных методов на границе.

## Синтаксис

```cpp
extern "C" {
struct Point { double x, y; };              // POD dlya peredachi po znacheniyu
int cpp_add(int a, int b) { return a + b; }
double point_length_squared(Point p);
}

struct Counter;                              // neprozrachnyy v C
extern "C" Counter* counter_create(int start);
extern "C" void counter_add(Counter* c, int delta);
extern "C" void counter_destroy(Counter* c);

using add_function = int (*)(int, int);      // ukazatel' na funkciyu C-ABI
add_function add = &cpp_add;
```

## Подводные камни

- **Соглашение о вызовах** (`cdecl` vs `stdcall`) на 64-битных Windows единое;
  на 32-битных нужно явно оговаривать (`__cdecl`/`__stdcall`).
- **Структуры по значению**: компилятор выбирает, вернуть в регистрах или
  через память (SysV vs Microsoft). Не полагайтесь на детали — передавайте
  указатель для больших структур.
- **Владение памятью**: кто выделил — тот и освобождает. Освобождать внутри
  одной CRT и удалять в другой (разные runtime) — ошибка.
- **`bool`/`char`/выравнивание**: в C ABI берите фиксированные типы
  (`std::int32_t`, `std::uint8_t`), а не `int`/`bool` без размера.
- Уберите `extern "C"` — и символ перестанет находиться из C (mangled).

## Задача

Определить C-ABI функции `cpp_add`, `point_length_squared`, а также
opaque-тип `Counter` с `create`/`add`/`value`/`destroy`. Вызвать `add` через
указатель на функцию, посчитать `3²+4²`, накопить счётчик 0→3→7 и вывести.

## Где применяется

- Плагины и разделяемые библиотеки, SDK на C для C++/Rust/Go/Python,
  системные и встраиваемые API, аудио/графические бэкенды (OpenGL, ALSA).

## Проверка

Вывод: `add(3, 4) = 7`, `point len^2 = 25`, `counter value = 10`,
`c abi done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex01_c_abi.cpp`. `static_assert` подтверждает, что `Point` — standard-layout
и trivially copyable (обязательно для передачи по C ABI).
