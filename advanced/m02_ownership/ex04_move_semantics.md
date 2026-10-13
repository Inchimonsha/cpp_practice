# m02_ownership/ex04_move_semantics — Move-семантика

> **Модуль:** `m02_ownership` · **Сложность:** ★★★★☆ · **Время:** 60–80 мин
> **Запуск:** `./run.ps1 m02_ownership/ex04_move_semantics`

## Теория (кратко)

`std::move(x)` — это не «перемещение», а **приведение к rvalue-ссылке**: оно лишь
разрешает компилятору выбрать перемещающий конструктор/присваивание вместо
копирующего. Перемещение «забирает» ресурс (указатели/буферы) и оставляет
источник в **валидном, но неопределённом** состоянии.

Категории значений: *lvalue* (именованный объект), *prvalue* (временный),
*xvalue* (`std::move`). «rvalue» = prvalue и xvalue.

| Выражение | Категория |
|---|---|
| `x` | lvalue |
| `x + 1` | prvalue |
| `std::move(x)` | xvalue |
| `std::forward<T>(x)` | сохраняет категорию |

## Синтаксис

```cpp
class Tracked {
public:
    Tracked(Tracked&& other) noexcept : id_(std::exchange(other.id_, -1)) {}
    Tracked& operator=(Tracked&& other) noexcept {
        id_ = std::exchange(other.id_, -1);
        return *this;
    }
private:
    int id_;
};

std::string source = "payload";
std::string target = std::move(source);   // source teper' pusto
```

## Подводные камни

- Использование объекта после `std::move` (кроме присваивания/reinit) — UB по
  контракту типа.
- `std::move` на `const`-объекте молча вызывает копирование (move-ctor требует
  не-const rvalue).
- `return std::move(local);` **вредит**: мешает NRVO (см. `ex06`).
- Перемещающие операции должны быть `noexcept`, иначе контейнеры копируют.

## Задача

В `ex04_move_semantics.cpp`:

1. реализуйте `Tracked` с перемещающими конструктором/присваиванием и
   запрещённым копированием;
2. покажите, что после `std::move` источник «пуст» (`id == -1`);
3. продемонстрируйте `std::move` для `std::string`;
4. покажите рост `std::vector<std::string>` с перемещением;
5. объясните (в выводе/комментарии), почему `std::move` — это каст, а не
   операция.

## Где применяется

- Передача больших объектов без копирования (буферы, строки, ресурсы).
- Реализация `push_back`/`emplace` в контейнерах.
- Фабрики и `return`-значения (вместе с elision — `ex06`).

## Проверка

Прогон печатает след перемещений и «пустое» состояние источника; завершается `0`.

### Решение

См. `ex04_move_semantics.cpp`.
