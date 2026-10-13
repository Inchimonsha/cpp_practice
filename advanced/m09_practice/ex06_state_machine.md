# ex06_state_machine — конечный автомат на std::variant

## Теория

Конечный автомат (FSM) — набор состояний, событий и правил перехода. В C++ его
удобно выразить типами:

- **Состояния** и **события** — отдельные типы, объединённые в `std::variant`.
- **Переход** — результат `std::visit` по паре (состояние, событие).
- Необработанные пары гасятся шаблонной перегрузкой-«по умолчанию» (событие
  игнорируется, состояние сохраняется).

Такой автомат проверяется на этапе компиляции (все ветки `visit` должны
существовать) и не требует таблиц переходов в рантайме.

## Синтаксис

```cpp
using State = std::variant<Menu, Playing, Paused, GameOver>;
using Event = std::variant<Start, Pause, Resume, Tick, Crash, Restart>;

struct Transition {
    State operator()(const Menu&, const Start&) const { return Playing{0}; }
    template <class S, class E>
    State operator()(const S& state, const E&) const { return state; }  // ignor
};

state = std::visit(Transition{}, state, event);
```

## Подводные камни

- Шаблонная «заглушка» не должна перекрывать точные перегрузки: нешаблонный
  `operator()` с точным совпадением типов выигрывает у шаблона.
- Возвращайте **новое** состояние, а не мутируйте — проще тестировать.
- Событие, недопустимое в состоянии, молча игнорируется; если это ошибка
  логики, верните `std::expected`/логируйте.
- Данные, живущие между состояниями (счёт), храните в самих состояниях, иначе
  придётся дублировать поля.
- Следите за порядком альтернатив, если полагаетесь на `index()`; лучше
  `std::visit`.

## Задача

Реализовать автомат игры `Menu → Playing → Paused → GameOver → Menu`. События:
`Start`, `Tick{points}`, `Pause`, `Resume`, `Crash`, `Restart`. Вести журнал
переходов «состояние --событие--> состояние» и проверить, что недопустимые
события не меняют состояние.

## Где применяется

- Игровые состояния, UI-навигация, сетевые протоколы, парсеры, конечные
  контроллеры, workflow/BPMN.

## Проверка

Вывод: журнал из 8 переходов и итоговое состояние; код возврата `0`, если ключевые
строки журнала совпали, иначе `1`.

## Решение

```cpp
#include <iostream>
#include <print>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace {

struct Menu {};
struct Playing { int score; };
struct Paused { int score; };
struct GameOver { int score; };

struct Start {};
struct Pause {};
struct Resume {};
struct Tick { int points; };
struct Crash {};
struct Restart {};

using State = std::variant<Menu, Playing, Paused, GameOver>;
using Event = std::variant<Start, Pause, Resume, Tick, Crash, Restart>;

std::string label(const State& state) {
    return std::visit([](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, Menu>) {
            return "Menu";
        } else if constexpr (std::is_same_v<T, Playing>) {
            return "Playing(" + std::to_string(value.score) + ")";
        } else if constexpr (std::is_same_v<T, Paused>) {
            return "Paused(" + std::to_string(value.score) + ")";
        } else {
            return "GameOver(" + std::to_string(value.score) + ")";
        }
    }, state);
}

std::string name(const Event& event) {
    return std::visit([](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, Start>) { return "Start"; }
        else if constexpr (std::is_same_v<T, Pause>) { return "Pause"; }
        else if constexpr (std::is_same_v<T, Resume>) { return "Resume"; }
        else if constexpr (std::is_same_v<T, Tick>) { return "Tick"; }
        else if constexpr (std::is_same_v<T, Crash>) { return "Crash"; }
        else { return "Restart"; }
    }, event);
}

struct Transition {
    State operator()(const Menu&, const Start&) const { return Playing{0}; }
    State operator()(const Playing& s, const Tick& e) const {
        return Playing{s.score + e.points};
    }
    State operator()(const Playing& s, const Pause&) const { return Paused{s.score}; }
    State operator()(const Paused& s, const Resume&) const { return Playing{s.score}; }
    State operator()(const Playing& s, const Crash&) const { return GameOver{s.score}; }
    State operator()(const Paused& s, const Crash&) const { return GameOver{s.score}; }
    State operator()(const GameOver&, const Restart&) const { return Menu{}; }

    template <class S, class E>
    State operator()(const S& state, const E&) const { return state; }
};

class Machine {
public:
    void dispatch(const Event& event) {
        const std::string before = label(state_);
        state_ = std::visit(Transition{}, state_, event);
        log_.push_back(before + " --" + name(event) + "--> " + label(state_));
    }

    const State& state() const { return state_; }
    const std::vector<std::string>& log() const { return log_; }

private:
    State state_ = Menu{};
    std::vector<std::string> log_;
};

}  // namespace

int main() {
    Machine machine;
    machine.dispatch(Start{});
    machine.dispatch(Tick{10});
    machine.dispatch(Tick{5});
    machine.dispatch(Pause{});
    machine.dispatch(Resume{});
    machine.dispatch(Crash{});
    machine.dispatch(Restart{});
    machine.dispatch(Tick{100});     // v Menu ignorietsya

    for (const auto& line : machine.log()) {
        std::println("{}", line);
    }
    std::println("final: {}", label(machine.state()));

    const auto& log = machine.log();
    const bool ok = log.size() == 8 &&
                    log[2] == "Playing(10) --Tick--> Playing(15)" &&
                    log[5] == "Playing(15) --Crash--> GameOver(15)" &&
                    log[7] == "Menu --Tick--> Menu";
    std::cout << "state machine done\n";
    return ok ? 0 : 1;
}
```
