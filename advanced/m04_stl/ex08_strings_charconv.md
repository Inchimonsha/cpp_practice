# m04_stl/ex08_strings_charconv — Строки, `charconv`, `format`

> **Модуль:** `m04_stl` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m04_stl/ex08_strings_charconv`

## Теория (кратко)

Для текста в современном C++ используют `std::string` (владение),
`std::string_view` (невладеющий вид) и конверсии чисел без исключений и без
локали — `std::from_chars`/`std::to_chars` из `<charconv>`. Форматирование —
через `std::format` (типобезопасно, без `printf`).

| Инструмент | Роль |
|---|---|
| `std::string_view` | невладеющее представление подстроки, O(1) suffix/prefix |
| `std::from_chars` | парсинг числа из диапазона `[ptr, end)` |
| `std::to_chars` | запись числа в буфер (без аллокаций) |
| `std::format` | сборка строки из значений |

## Синтаксис

```cpp
std::string_view sv = "  42";
// trim (from_chars ne propuskaet probely)
while (!sv.empty() && sv.front() == ' ') { sv.remove_prefix(1); }

int value{};
const auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);

char buffer[32];
const auto [end, ec2] = std::to_chars(buffer, buffer + sizeof(buffer), 3.14);
std::string text{buffer, end};

std::string line = std::format("{} = {}", "answer", 42);
```

## Подводные камни

- `string_view` не владеет: указывает на строку, которая должна жить дольше.
- `from_chars` **не пропускает** пробелы и по умолчанию парсит без знака `+`;
  проверяйте `ec` и `ptr == end` (весь вход разобран?).
- `std::stoi` бросает исключения и использует локаль — для парсинга горячего
  пути `from_chars` предпочтительнее.
- `to_chars` для `double` даёт **кратчайшее** представление, но не округляет по
  формату (`format` — для форматирования).

## Задача

В `ex08_strings_charconv.cpp`:

1. обрежьте пробелы у `string_view` и распарсите число `from_chars`;
2. проверьте обработку ошибки парсинга (нечисловой ввод);
3. запишите `int` и `double` в буфер через `to_chars`;
4. соберите строку через `format`;
5. покажите `remove_prefix`/`remove_suffix` для разбора.

## Где применяется

- Парсинг конфигов/протоколов/CSV без исключений.
- Ноль-копийные интерфейсы (`string_view` в параметрах).
- Логи и отчёты через `format`.

## Проверка

Прогон печатает результаты парсинга, форматирования и разбора; завершается `0`.

### Решение

См. `ex08_strings_charconv.cpp`.
