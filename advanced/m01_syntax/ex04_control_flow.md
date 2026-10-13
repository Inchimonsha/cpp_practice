# m01_syntax/ex04_control_flow — Управление потоком

> **Модуль:** `m01_syntax` · **Сложность:** ★★☆☆☆ · **Время:** 40–60 мин
> **Запуск:** `./run.ps1 m01_syntax/ex04_control_flow`

## Теория (кратко)

Управляющие конструкции задают порядок выполнения. Современный C++ позволяет
объявлять переменную прямо в условии (`if`/`switch` с init-statement), обходить
контейнер по элементам (`range-for`) и раскладывать пары на компоненты
(structured bindings).

| Конструкция | Особенность |
|---|---|
| `if`/`else` с init | `if (auto it = find(); it != end)` — переменная видна только внутри |
| `switch` с init | `switch (auto x = f(); x)` + `[[fallthrough]]` |
| `for` / range-for | range-for копирует, если не `&`/`const&` |
| `while` / `do-while` | `do` гарантирует один проход |
| `break`/`continue` | выход / следующая итерация |
| structured bindings | `auto [k, v] : map` |

## Синтаксис

```cpp
if (auto n = parse(text); n.has_value()) { use(*n); }

switch (auto cmd = read(); cmd) {
    case 'a': attack(); [[fallthrough]];
    case 'b': block();  break;
    default:  break;
}

for (const auto& [key, value] : table) { use(key, value); }

do { step(); } while (running);
```

## Подводные камни

- Range-for по контейнеру из временного выражения продлевает ему жизнь, но
  `for (auto x : make_vec())` копирует каждый элемент — используйте `const auto&`.
- `switch` только по целочисленным/перечислимым типам; `break` забывать нельзя.
- `goto`, выпрыгивающий через инициализацию переменной, запрещён.
- Бесконечный `for(;;)` без выхода — проверяйте условие внутри.

## Задача

В `ex04_control_flow.cpp`:

1. реализуйте `fizzbuzz(int n)` через `if`/`else` и выведите 1..15;
2. классификацию оценки `grade(int)` через `switch` с `[[fallthrough]]`;
3. обход `std::map` через range-for + structured bindings;
4. имитацию меню команд через `do-while` (детерминированный список команд);
5. `if` с init-statement для безопасного поиска.

## Где применяется

- `switch` по коду команды/сообщения — сетевые протоколы, парсеры, меню.
- Range-for — штатный обход сущностей, компонентов, тайлов.
- `if` с init — поиск в контейнере без «висящих» итераторов в области видимости.

## Проверка

Прогон печатает FizzBuzz, классификацию, карту и результат меню; завершается `0`.

### Решение

См. `ex04_control_flow.cpp`.
