# m03_templates/ex07_crtp_type_erasure — CRTP и стирание типов

> **Модуль:** `m03_templates` · **Сложность:** ★★★★★ · **Время:** 70–90 мин
> **Запуск:** `./run.ps1 m03_templates/ex07_crtp_type_erasure`

## Теория (кратко)

**CRTP** (Curiously Recurring Template Pattern) — базовый класс знает тип
наследника: `struct D : Base<D>`. Даёт **статический полиморфизм** (без vtable),
миксины и «добавь оператор по одному методу».

**Type erasure** (стирание типов) — приём, когда разные типы приводятся к
единому интерфейсу без общего базового класса: внутри прячется виртуальная
модель, снаружи — обычный класс. Так устроен `std::function`.

| Приём | Полиморфизм | Цена |
|---|---|---|
| CRTP | статический (compile-time) | нет vtable, но рост кода |
| virtual | динамический (run-time) | vtable, указатель |
| type erasure | динамический, интерфейс без наследования | vtable + аллокация модели |

## Синтаксис

```cpp
template <class Derived>
struct Equality {
    friend bool operator!=(const Derived& a, const Derived& b) { return !(a == b); }
};
struct Point : Equality<Point> {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

class AnyGreeter {                       // type erasure
public:
    template <class T> explicit AnyGreeter(T obj)
        : self_(std::make_unique<Model<T>>(std::move(obj))) {}
    std::string greet() const { return self_->greet(); }
private:
    struct Concept { virtual ~Concept() = default; virtual std::string greet() const = 0; };
    template <class T> struct Model : Concept {
        T obj_;
        explicit Model(T o) : obj_(std::move(o)) {}
        std::string greet() const override { return obj_.greet(); }
    };
    std::unique_ptr<Concept> self_;
};
```

## Подводные камни

- CRTP-база не может обращаться к членам `Derived` напрямую — нужен
  `static_cast<Derived*>(this)`.
- `Base<D>` с виртуальным деструктором бессмыслен; CRTP обычно без vtable.
- Type erasure требует копирования/перемещения модели; для копируемости нужен
  `clone()`.
- Не применяйте CRTP ради экономии vtable без замеров (см. `m07_performance`).

## Задача

В `ex07_crtp_type_erasure.cpp`:

1. CRTP `Equality<D>` — даёт `operator!=` из `operator==` наследника;
2. примените к `Point` и проверьте оба оператора;
3. CRTP-миксин `Counted<D>` со счётчиком экземпляров;
4. `AnyGreeter` (type erasure) над произвольными типами с методом `greet()`;
5. сложите разные «приветствия» в один `vector<AnyGreeter>` и выведите.

## Где применяется

- CRTP: `std::enable_shared_from_this`, миксины сравнения, сериализации, CRTP-`Visitor`.
- Type erasure: `std::function`, `std::any`, колбэки, «интерфейсы без наследования»,
  подписки в event bus (ср. `m09_practice/ex05_event_bus`).

## Проверка

Прогон печатает результаты сравнения, счётчик миксина и приветствия разных
типов; завершается `0`.

### Решение

См. `ex07_crtp_type_erasure.cpp`.
