# m02_ownership/ex05_perfect_forwarding — Идеальная передача

> **Модуль:** `m02_ownership` · **Сложность:** ★★★★☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m02_ownership/ex05_perfect_forwarding`

## Теория (кратко)

**Forwarding reference** — это `T&&`, где `T` выводится. В отличие от обычной
rvalue-ссылки, он принимает и lvalue, и rvalue. `std::forward<T>(x)` передаёт
аргумент дальше, **сохраняя** его категорию (lvalue останется lvalue, rvalue —
rvalue). Это фундамент «прозрачных» обёрток и фабрик.

Ключ — **свёртка ссылок** (reference collapsing): `& &`→`&`, `&& &`→`&`,
`& &&`→`&`, `&& &&`→`&&`.

| Deduciruemoe `T` из `T&&` | Итог типа | Аргумент |
|---|---|---|
| `X&` | `X&` | lvalue |
| `X` | `X&&` | rvalue |

## Синтаксис

```cpp
template <class T>
void wrapper(T&& arg) {                // forwarding reference
    consume(std::forward<T>(arg));     // sohranyaet kategoriyu
}

template <class T, class... Args>
T make(Args&&... args) {               // fabrika
    return T(std::forward<Args>(args)...);
}
```

## Подводные камни

- `T&&` — forwarding reference **только** если `T` выводится в этом шаблоне. В
  `void f(std::vector<int>&& v)` это обычная rvalue-ссылка.
- `auto&&` в range-for — распространённый forwarding reference.
- `std::forward<T>` применяйте **один раз** и только к тому параметру, чей `T`
  выведен; двойное использование forwarding-аргумента опасно.
- Забыли `std::forward` → всё превращается в lvalue (лишнее копирование).

## Задача

В `ex05_perfect_forwarding.cpp`:

1. напишите перегрузки `classify(int&)` / `classify(int&&)` для показа категории;
2. обёртку `template <class T> void relay(T&&)`, вызывающую `classify` через
   `std::forward`;
3. фабрику `make<T>(Args&&...)`, возвращающую `T` (например, `std::string`,
   `std::unique_ptr`);
4. печатайте «свёрнутый» тип через `if constexpr` + `std::is_*_reference_v`;
5. покажите разницу между `relay(x)` и `relay(std::move(x))`.

## Где применяется

- Фабрики (`make_unique`/`make_shared`/`emplace_back`).
- Обёртки логирования, метрик, ретраев, «middleware».
- Любой API, который должен передавать аргументы дальше без потери категории.

## Проверка

Прогон печатает категории аргументов, свёрнутые типы и созданный объект;
завершается `0`.

### Решение

См. `ex05_perfect_forwarding.cpp`.
