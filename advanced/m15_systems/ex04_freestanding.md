# m15_systems/ex04_freestanding — свободно стоящая среда (-ffreestanding)

> Модуль m15_systems · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m15_systems/ex04_freestanding`
>
> Здесь те же приёмы показаны в hosted-режиме (чтобы запускаться). Реальная
> сборка: `g++ -ffreestanding -fno-exceptions -fno-rtti -c kernel.cpp`
> (проверено — `__STDC_HOSTED__` становится `0`).

## Теория

**Freestanding-реализация** (C++, [basic.stc], `__STDC_HOSTED__ == 0`) не
обязана предоставлять весь стандартный рантайм: нет `main`, `iostream`, файлов,
`new`/`delete`, исключений, потоков. Доступен **freestanding-подмножество
заголовков** (`<cstdint>`, `<cstddef>`, `<type_traits>`, `<atomic>`,
`<bit>`, `<limits>`, ...). Это основа для:

- ядер ОС и загрузчиков, embedded/микроконтроллеров, бутлоадеров;
- `-nostdlib`/custom runtime, static-библиотек для нестандартных сред.

Ключевые ограничения и приёмы: **MMIO** (регистры устройств по фиксированным
адресам, `volatile`), **таблица векторов прерываний** (массив `extern "C"`
функций), **отсутствие исключений** (`-fno-exceptions`, коды возврата),
**отсутствие RTTI** (`-fno-rtti`).

## Синтаксис

```cpp
// kernel.cpp — bez iostream, bez main; tochka vhoda zadayotsya linkeru:
extern "C" void kernel_main();

std::uint32_t* base() { return registers_.data(); }   // MMIO-registry
void write(std::size_t i, std::uint32_t v) { base()[i] = v; }

extern "C" void isr_timer() { ++g_timer_count; }
using isr_function = void (*)();
const std::array<isr_function, 2> kVectorTable{&isr_timer, &isr_keyboard};
```

Сборка (проверено):

```bash
g++ -std=c++23 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -c kernel.cpp
# __STDC_HOSTED__ == 0
```

## Подводные камни

- **`-ffreestanding` не мешает случайно включить `<iostream>`**: заголовки
  скомпилируются, но при линковке без рантайма упадёт; держите дисциплину.
- **`volatile` обязателен для MMIO**, иначе `-O2` выбросит/переупорядочит
  доступ к «регистру».
- **Точка входа**: в hosted это `main`, в ядре — `_start`/`kernel_main`,
  задаваемая линкер-скриптом; не забудьте про статическую инициализацию
  (в некоторых средах её нужно вызывать вручную).
- **Исключения и `-fno-exceptions`**: любой `throw` станет `terminate`/ошибкой
  компиляции — используйте `noexcept` и коды ошибок.
- **`new`/`delete` недоступны**: подключайте собственную арену/пул или
  заказной `operator new` поверх `mmap`/статического буфера.
- **Стек и выравнивание**: таблица векторов, выравнивание регистров и порядок
  байт (`std::endian`) зависят от ABI платформы.

## Задача

Смоделировать MMIO-регистры (запись/чтение `STATUS`/`DATA`), таблицу векторов
прерываний из двух `extern "C"` обработчиков и вызвать их по указателю из
таблицы. Проверить значения.

## Где применяется

- Ядра и драйверы (Linux, FreeRTOS, Zephyr), микроконтроллеры (ESP32, Cortex-M),
  бутлоадеры, гипервизоры, прошивки, WASM/WASI-совместимый код.

## Проверка

Вывод: `mmio regs: status = 1, data = 0x2A`,
`isr timer count = 3, keyboard count = 2`, `freestanding ok`. Код возврата `0`
при совпадении, иначе `1`.

## Решение

См. `ex04_freestanding.cpp`. В hosted-режиме используется `iostream` только для
вывода отчёта; вся «ядерная» часть независима от рантайма.
