# m04_stl/ex11_sum_product_types — Суммы и произведения типов

> **Модуль:** `m04_stl` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m04_stl/ex11_sum_product_types`

## Теория (кратко)

**`optional<T>`** — либо значение, либо «пусто». **`variant<...>`** — «одно из»
(типобезопасная сумма). **`any`** — значение любого типа (безопасная замена
`void*`). **`tuple`/`pair`** — произведение типов (несколько значений вместе).

| Тип | Семантика | Проверка |
|---|---|---|
| `optional<T>` | есть/нет | `has_value()`, `*`, `value_or` |
| `variant<A,B>` | ровно один из | `holds_alternative<T>`, `get`, `visit` |
| `any` | любой тип | `any_cast<T>(&a)` |
| `tuple<...>` | много значений | `get<>`, structured bindings |

## Синтаксис

```cpp
std::optional<int> parse(int x);           // nullopt ili znachenie

std::variant<int, std::string> v = 42;
std::visit([](auto&& x) { use(x); }, v);

std::any a = std::string{"text"};
if (auto* p = std::any_cast<std::string>(&a)) { use(*p); }

auto [id, name] = std::tuple{1, std::string{"hero"}};
```

## Подводные камни

- `variant` хранит **самый большой** альтернативный тип по размеру (плюс
  выравнивание); для тяжёлых типов это может быть неожиданно большим.
- `std::get<T>(v)` бросает `bad_variant_access` при несовпадении; для безопасного
  доступа — `std::get_if<T>(&v)`.
- `optional<T>::value()` бросает при пустом; `value_or` — нет.
- `any_cast` по указателю возвращает `nullptr` вместо исключения.
- `std::visit` должен покрывать **все** альтернативы (иначе не компилируется).

## Задача

В `ex11_sum_product_types.cpp`:

1. `parse_positive(int) -> std::optional<int>` и обработка пустого;
2. `variant<int, std::string, double>` и вывод через `std::visit`;
3. безопасный доступ `std::get_if`/`holds_alternative`;
4. `std::any` + `any_cast` (успех и неуспех);
5. `std::tuple` со structured bindings и `std::apply`.

## Где применяется

- `optional` — «может отсутствовать» (поиск, парсинг, настройки).
- `variant` — сообщения/события/узлы AST с разными полезными нагрузками.
- `any` — параметры плагинов, рефлексия «бедного человека».
- `tuple` — группировка результатов без объявления struct.

## Проверка

Прогон печатает результаты для всех «сумм/произведений»; завершается `0`.

### Решение

См. `ex11_sum_product_types.cpp`.
