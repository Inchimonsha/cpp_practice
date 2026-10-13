# m13_interop_ffi/ex06_rust_ffi — конвенции Rust FFI

> Модуль m13_interop_ffi · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m13_interop_ffi/ex06_rust_ffi`
>
> Тулчейна Rust (`rustc`/`cargo`) в среде нет; показаны **конвенции**, которые
> Rust использует на границе, и их точные C++-эквиваленты.

## Теория

Rust и C++ связываются через **C ABI**. Ключевые конструкции Rust:

- **`#[repr(C)]`** — гарантирует раскладку структуры как в C (порядок и
  выравнивание полей). Аналог — C++ `standard-layout`.
- **`extern "C" fn`** + **`#[no_mangle]`** — функция с C-ABI и без mangling.
- **`Option<Box<T>>` как nullable-указатель** — `Box` это уникальный указатель;
  `None` в границах оптимизируется до `NULL` (null-pointer optimization).
- **Владение через границу** — «создать» возвращает указатель, «освободить»
  (обычно `Box::from_raw`) забирает владение обратно; вызывающая сторона не
  должна освобождать чужой аллокатор.

Тот же C ABI, что в `ex01`/`ex02`: Rust-библиотеку можно вызвать из C++, и
наоборот, без рантайма Rust.

## Синтаксис

```cpp
// Rust: #[repr(C)] struct Widget { id: u32, weight: f64 }
struct Widget { std::uint32_t id; double weight; };

// Rust: #[no_mangle] pub extern "C" fn widget_new(...) -> *mut Widget
Widget* widget_new(std::uint32_t id, double weight) {
    if (weight < 0.0) return nullptr;   // None (Option<Box<Widget>>)
    return new Widget{id, weight};
}
void widget_free(Widget* w) { delete w; }   // Rust: Box::from_raw(w).drop
```

## Подводные камни

- **`#[repr(Rust)]` (по умолчанию) нестабилен** между версиями и переупорядочивает
  поля — пересекать границу можно только через `#[repr(C)]`.
- **`String`/`Vec` — не C-типы** (у них свой аллокатор и layout). Передавайте
  `(pointer, length)` и явно оговаривайте владение/освобождение.
- **`Option<T>` не всегда=NULL**: null-pointer optimization работает только для
  «указателеподобных» типов; `Option<u32>` имеет отдельный discriminant.
- **`bool` в Rust = байт**, а `bool` C++ не гарантирует размер; на границе
  используйте `u8`/`std::uint8_t`.
- **Паники через `extern "C"`** — UB/аборт: ловите их (`catch_unwind`) или
  возвращайте коды ошибок; аналог C++-исключений через C ABI.
- **Разные аллокаторы**: освобождать память должен тот, кто выделил
  (`widget_free` внутри Rust), иначе — порча кучи.
- **Инструменты**: `bindgen` (C→Rust), `cbindgen` (Rust→C/C++), `cxx`/`autocxx`
  (безопасные мосты).

## Задача

Реализовать «Rust-стиль» API на C++: `#[repr(C)] struct Widget`, фабрику
`widget_new` (возвращает `nullptr` при отрицательном весе — как `None`),
геттер и `widget_free` (возврат владения). Проверить раскладку через
`static_assert` и продемонстрировать оба случая (`Some`/`None`).

## Где применяется

- Высокопроизводительные ядра на Rust в C++-коде (и наоборот), безопасные
  системные библиотеки, WASM-модули (`wasm-bindgen`), встраивание Rust в
  игры/СУБД, замена C-библиотек.

## Проверка

Вывод: `repr(C) widget is standard-layout`,
`widget_new(1, 2.5) -> weight 2.5`, `widget_new(2, -1) -> nullptr (None)`,
`rust ffi (simulated conventions) done`. Код возврата `0` при совпадении,
иначе `1`.

## Решение

См. `ex06_rust_ffi.cpp`. Код повторяет C-ABI-контракт, который сгенерировал бы
Rust с `#[repr(C)]` и `extern "C"`; реальная сборка потребовала бы `rustc` +
`cargo` и `extern crate`.
