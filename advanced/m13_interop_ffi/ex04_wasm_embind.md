# m13_interop_ffi/ex04_wasm_embind — граница JS↔WebAssembly (embind)

> Модуль m13_interop_ffi · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m13_interop_ffi/ex04_wasm_embind`
>
> В среде нет компилятора **Emscripten** (`emcc`), поэтому пример демонстрирует
> **модель** границы WASM: линейную память и таблицу экспортов.

## Теория

WebAssembly (WASM) — переносимый бинарный формат, исполняемый в песочнице
(браузер/Node). C++ компилируется в WASM через **Emscripten**. Граница между
JS и WASM пересекается значениями `i32`/`i64`/`f32`/`f64`, а сложные типы
живут в **линейной памяти** (большой `ArrayBuffer`), в которую JS и WASM
пишут по смещениям (указателям/индексам).

**embind** — обвязка Emscripten, регистрирующая C++-функции и классы для JS:

```cpp
EMSCRIPTEN_BINDINGS(module) {
    emscripten::function("add", &add);
    emscripten::class_<Vec2>("Vec2").constructor<double, double>()...;
}
```

Аналоги: `wasm-bindgen` (Rust), `webidl-bindings`. Здесь недоступны — поэтому
модель повторяет ключевые понятия: экспорты по имени, диспетчеризация,
линейная память и запись/чтение по смещению.

## Синтаксис

```cpp
class LinearMemory {
public:
    explicit LinearMemory(std::size_t words) : data_(words, 0) {}
    void store(std::size_t offset, std::span<const std::int32_t> values);
    std::int32_t load(std::size_t offset) const;
private:
    std::vector<std::int32_t> data_;
};

class WasmModule {
public:
    using Export = std::function<std::int32_t(std::span<const std::int32_t>)>;
    void expose(const std::string& name, Export fn);       // registraciya (embind)
    std::int32_t call(const std::string& name,
                      std::span<const std::int32_t> args) const;   // vyzov iz JS
    LinearMemory& memory();
};

module.expose("add", [](std::span<const std::int32_t> a) { return a[0] + a[1]; });
```

## Подводные камни

- **Границы значений**: через границу ходят только числа; строки/объекты —
  через память + смещение, с ручным управлением длиной.
- **Рост памяти**: линейная память может расти (`memory.grow`), и старые
  буферные представления в JS становятся недействительными — берите новый
  `Memory.buffer` после роста.
- **Выравнивание и endianness**: WASM little-endian; невыровненный доступ
  медленнее или недоступен для некоторых типов.
- **Владение объектами**: WASM-объект нельзя «просто вернуть» — JS держит
  дескриптор и обязан вызывать `.delete()` (embind) или `drop` (wasm-bindgen).
- **Отсутствие исключений/`std::string` в ABI**: если не включён
  `-fexceptions`, `throw` приводит к прерыванию; строки маршалятся вручную.
- Модель **не измеряет** реальную производительность и не заменяет `emcc`.

## Задача

Смоделировать модуль с экспортами: `add` (сумма двух чисел) и `sum_memory`
(сумма `n` слов из линейной памяти по смещению). Записать `{1,2,3,4}` в
память, вызвать оба экспорта из «стороны JS» и вывести результаты.

## Где применяется

- Web-приложения (игры, редакторы, CAD), переносимые плагины в браузере,
  серверный WASM (`wasmtime`, WASI), «вычислительные ядра» на C++/Rust для JS.

## Проверка

Вывод: `wasm (simulated): exposed add, sum_memory`, `js: add(2, 3) = 5`,
`js: sum_memory(0, 4) = 10`, `wasm embind (simulated) done`. Код возврата `0`
при совпадении, иначе `1`.

## Решение

См. `ex04_wasm_embind.cpp`. Это модель: `expose` соответствует регистрации
embind, `call` — вызову из JS, `LinearMemory` — линейной памяти модуля.
