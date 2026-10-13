# ex09_lua_bindings — привязки в стиле Lua C API

## Теория

Lua встраивают через C API: значения кладутся на **стек**, нативные функции
читают аргументы и кладут результат. Ключевые приёмы, которые нужно воспроизвести
на C++:

- **Стек значений** (`lua_pushnumber`, `lua_tonumber`, `lua_settop`) — единый
  интерфейс между C++ и скриптом, скрывающий типы.
- **RAII-восстановление стека**: `lua_settop(L, base)` в деструкторе гарантирует,
  что функция не «насорит» на стеке при выходе — в т.ч. по исключению. Так же
  устроены чужие `StackGuard`/`stack::restore`.
- **Проверка типов** через `lua_tonumber`/`luaL_checknumber`: ошибка превращается
  в `lua_error`, а на стороне C++ — в `std::expected`/исключение.
- **Регистрация** нативных функций по имени (`lua_register`).

Здесь Lua-библиотека недоступна, поэтому модель API реализована на `std::variant`
(значения), `std::span` (аргументы) и `std::expected` (ошибки).

## Синтаксис

```cpp
using Value = std::variant<double, std::string, bool>;

state.register_function("add", [](std::span<const Value> args)
    -> std::expected<Value, std::string> { /* ... */ });

{
    StackGuard guard(state);          // zapomnil vershinu
    state.push(Value{1.0});
    // ... pri vyhode (v tom chisle po isklyucheniyu) settop(saved)
}

auto result = state.call("add", {Value{2.0}, Value{3.0}});
```

## Подводные камни

- Индекс стека: Lua считает от 1 (и отрицательно с верха); следите за базой при
  восстановлении — `settop(base)` сохраняет аргументы ниже.
- Стек Lua ограничен: не оставляйте на нём значения между вызовами.
- Проверка типов обязательна: приведение «наугад» даёт UB/мусор.
- Граница владения: строки/таблицы, попавшие в скрипт, могут быть собраны GC —
  копируйте или удерживайте ссылки.
- Ошибки через `longjmp` (настоящий Lua C API) разрушают C++-стек: вызывайте
  `lua_pcall` и не позволяйте исключениям утекать через C-границу.
- Для безопасности ограничивайте набор доступных функций (sandboxing).

## Задача

Реализовать `LuaState` с `push/pop/top/settop`, реестром нативных функций и
`call`, возвращающим `std::expected<Value, std::string>`. Зарегистрировать `add`
и `concat`, продемонстрировать вызовы, RAII-`StackGuard`, вызов несуществующей
функции и ошибку типа.

## Где применяется

- Встраивание скриптов в игровые движки (геймплей, конфиги, моды), плагины,
  настройка поведения без пересборки, оверлеи и редакторы.

## Проверка

Вывод: результаты `add(2,3)=5` и `concat("lu","a")="lua"`, глубина стека до/внутри
/после, ошибки `no function 'nope'` и `expected number`; код возврата `0`, иначе
`1`.

## Решение

```cpp
#include <cstddef>
#include <expected>
#include <format>
#include <functional>
#include <iostream>
#include <print>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

namespace {

using Value = std::variant<double, std::string, bool>;

std::string repr(const Value& value) {
    return std::visit([](const auto& item) -> std::string {
        using T = std::decay_t<decltype(item)>;
        if constexpr (std::is_same_v<T, bool>) {
            return item ? "true" : "false";
        } else {
            return std::format("{}", item);
        }
    }, value);
}

std::expected<double, std::string> as_number(const Value& value) {
    if (const auto* number = std::get_if<double>(&value)) { return *number; }
    return std::unexpected("expected number");
}

std::expected<std::string, std::string> as_string(const Value& value) {
    if (const auto* text = std::get_if<std::string>(&value)) { return *text; }
    return std::unexpected("expected string");
}

class LuaState {
public:
    using Native = std::function<std::expected<Value, std::string>(
        std::span<const Value>)>;

    void push(Value value) { stack_.push_back(std::move(value)); }
    Value pop() {
        Value top = std::move(stack_.back());
        stack_.pop_back();
        return top;
    }
    std::size_t top() const { return stack_.size(); }
    void settop(std::size_t level) {
        if (level < stack_.size()) { stack_.resize(level); }
    }

    template <class F>
    void register_function(std::string name, F fn) {
        natives_.emplace(std::move(name), Native(std::move(fn)));
    }

    std::expected<Value, std::string> call(const std::string& name,
                                           std::vector<Value> args) const {
        const auto it = natives_.find(name);
        if (it == natives_.end()) { return std::unexpected("no function '" + name + "'"); }
        return it->second(std::span<const Value>(args));
    }

private:
    std::vector<Value> stack_;
    std::unordered_map<std::string, Native> natives_;
};

class StackGuard {
public:
    explicit StackGuard(LuaState& state) : state_(state), saved_(state.top()) {}
    ~StackGuard() { state_.settop(saved_); }
    StackGuard(const StackGuard&) = delete;
    StackGuard& operator=(const StackGuard&) = delete;

private:
    LuaState& state_;
    std::size_t saved_;
};

}  // namespace

int main() {
    LuaState state;

    state.register_function("add", [](std::span<const Value> args)
                                -> std::expected<Value, std::string> {
        if (args.size() != 2) { return std::unexpected("add expects 2 arguments"); }
        const auto a = as_number(args[0]);
        if (!a) { return std::unexpected(a.error()); }
        const auto b = as_number(args[1]);
        if (!b) { return std::unexpected(b.error()); }
        return Value{*a + *b};
    });

    state.register_function("concat", [](std::span<const Value> args)
                                     -> std::expected<Value, std::string> {
        std::string out;
        for (const Value& arg : args) {
            const auto text = as_string(arg);
            if (!text) { return std::unexpected(text.error()); }
            out += *text;
        }
        return Value{out};
    });

    if (auto sum = state.call("add", {Value{2.0}, Value{3.0}})) {
        std::println("add = {}", repr(*sum));
    }

    if (auto text = state.call("concat", {Value{std::string{"lu"}},
                                          Value{std::string{"a"}}})) {
        std::println("concat = {}", repr(*text));
    }

    const std::size_t before = state.top();
    {
        StackGuard guard(state);
        state.push(Value{1.0});
        state.push(Value{2.0});
        std::println("stack inside = {}", state.top());
    }
    std::println("stack before = {}, after = {}", before, state.top());

    if (auto missing = state.call("nope", {}); !missing) {
        std::println("call error = {}", missing.error());
    }
    if (auto bad = state.call("add", {Value{std::string{"x"}}, Value{1.0}}); !bad) {
        std::println("type error = {}", bad.error());
    }

    const bool ok = before == 0 && state.top() == 0;
    std::cout << "lua bindings done\n";
    return ok ? 0 : 1;
}
```
