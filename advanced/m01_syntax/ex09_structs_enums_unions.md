# m01_syntax/ex09_structs_enums_unions — Структуры, перечисления, union

> **Модуль:** `m01_syntax` · **Сложность:** ★★★☆☆ · **Время:** 40–60 мин
> **Запуск:** `./run.ps1 m01_syntax/ex09_structs_enums_unions`

## Теория (кратко)

`struct` объединяет данные (агрегат). `enum class` — безопасное перечисление
(без неявного преобразования в `int`). `union` хранит в одной памяти одно из
полей — мощно, но опасно (активен только один член). В современном C++ вместо
`union` обычно берут `std::variant`.

| Средство | Назначение |
|---|---|
| `struct` + designated init | составные данные с понятными полями |
| `enum class` | типобезопасные константы |
| bitfields | упаковка флагов в слово |
| `union` | одна память на несколько типов (низкий уровень) |
| `std::variant` | типобезопасная «сумма типов» |

## Синтаксис

```cpp
struct Config { int width; int height; bool fullscreen; };
Config c{.width = 1280, .height = 720, .fullscreen = false};

enum class State { idle, run, jump };

struct Flags {            // bitfields
    unsigned visible : 1;
    unsigned solid   : 1;
    unsigned layer   : 4;
};

std::variant<int, std::string> value = 42;
```

## Подводные камни

- Обычный `enum` протекает в `int`; `enum class` — нет (нужен `static_cast`).
- Порядок полей в bitfield и выравнивание зависят от платформы — не для форматов
  обмена.
- Чтение «неактивного» члена `union` — UB (кроме разрешённых случаев
  common initial sequence).
- `std::variant` всегда хранит ровно одно значение; `std::monostate` — пустое.

## Задача

В `ex09_structs_enums_unions.cpp`:

1. заведите `struct RenderConfig` и инициализируйте через designated init;
2. определите `enum class State` и `to_string` для него (через `switch`);
3. покажите bitfield-флаги и посчитайте упакованное значение;
4. используйте `std::variant<int, std::string, double>` и `std::visit`;
5. в комментарии укажите, почему `union` здесь заменён на `variant`.

## Где применяется

- `enum class` — состояния, типы событий, категории ресурсов.
- Bitfields — заголовки бинарных форматов, флаги тайлов/сущностей.
- `std::variant` — AST парсера, значения конфигураций, сетевые сообщения.

## Проверка

Прогон печатает конфиг, состояние, флаги и значения variant; завершается `0`.

### Решение

См. `ex09_structs_enums_unions.cpp`.
