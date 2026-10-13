# m05_errors/ex03_expected_monadic — `std::expected` и монадические операции

> **Модуль:** `m05_errors` · **Сложность:** ★★★★☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m05_errors/ex03_expected_monadic`

## Теория (кратко)

`std::expected<T, E>` хранит **либо** значение `T`, **либо** ошибку `E` — без
исключений и без накладных расходов `optional`+`error_code`. Монадические
методы позволяют собирать **цепочки** обработки, где ошибка «пробрасывается»
автоматически:

| Метод | Действие |
|---|---|
| `and_then(f)` | продолжить, если есть значение (`f` возвращает `expected`) |
| `transform(f)` | преобразовать значение (обёртка не нужна) |
| `or_else(f)` | обработать ошибку, вернуть `expected` |
| `transform_error(f)` | преобразовать ошибку |
| `value_or(x)` | значение или запасное |

## Синтаксис

```cpp
std::expected<int, std::string> parse(std::string_view s);

auto result = parse("42")
    .and_then([](int v) -> std::expected<int, std::string> {
        return v > 0 ? std::expected<int, std::string>{v}
                     : std::unexpected(std::string{"must be positive"});
    })
    .transform([](int v) { return v * 2; });
```

## Подводные камни

- `and_then` требует, чтобы `f` возвращала `expected`; `transform` — чтобы
  возвращала **значение**.
- `.value()` бросает `bad_expected_access` при ошибке; на горячем пути
  предпочитайте `value_or`/`and_then`.
- Тип ошибки может быть любым (`std::string`, `std::error_code`, свой `enum`); для
  системных кодов удобен `std::error_code`, для понятных сообщений — строка.
- Не смешивайте `expected` и исключения в одной и той же цепочке без нужды.

## Задача

В `ex03_expected_monadic.cpp`:

1. напишите `parse_int(string_view) -> expected<int, error_code>`;
2. добавьте `validate_positive` (возвращает `expected`);
3. соберите цепочку `and_then`/`transform`;
4. обработайте ошибки через `or_else`/`transform_error`;
5. примените `value_or` как запасной результат.

## Где применяется

- Парсеры/валидаторы, где ошибка — обычное дело, а не исключение.
- API-слои, не желающие бросать исключения через границы (`extern "C"`, C ABI).
- Композируемые операции над результатами (пайплайны).

## Проверка

Прогон печатает результаты успешной и ошибочных цепочек; завершается `0`.

### Решение

См. `ex03_expected_monadic.cpp`.
