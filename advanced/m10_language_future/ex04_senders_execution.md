# m10_language_future/ex04_senders_execution — sender/receiver и `std::execution`

## Теория

`sender/receiver` — модель асинхронности из C++26 (`std::execution`). Он
разделяет **описание** работы и её **запуск**:

- **sender** — ленивое описание вычисления с известными «сигнатурами
  завершения»: `set_value(...)`, `set_error(...)`, `set_stopped()`.
- **receiver** — приёмник этих трёх событий.
- `connect(sender, receiver)` создаёт **operation state**; `start()` запускает.
- Алгоритмы (`then`, `when_all`, `upon_error`, `bulk`, ...) **композируют**
  сендеры, не запуская их.

Это позволяет строить пайплайны, которые затем исполняются на выбранном
scheduler'е (пул потоков, GPU, `inline_scheduler`) без изменения их описания.

В GCC 16.2 `std::execution` ещё нет (`__cpp_lib_senders` не определён). В
`m06_concurrency/ex06` разобран базовый `just`/`then`; здесь модель расширена
**каналом ошибок**, `upon_error` (восстановление), `when_all` (объединение двух
сендеров) и `sync_wait` (синхронный запуск в `std::expected`).

## Синтаксис

```cpp
auto s = exec::then(exec::just(41), [](int v) { return v + 1; });
auto e = exec::upon_error(exec::just_error("boom"), [](std::string) { return 100; });
auto all = exec::when_all(s, e, [](int a, int b) { return a + b; });
std::expected<int, std::string> r = exec::sync_wait(all);   // 142
```

## Подводные камни

- Сендер **ленив**: `connect`/`start` — только явный запуск; алгоритмы ничего не
  вычисляют, пока не подключён receiver и не вызван `start()`.
- Receiver обязан поддержать все три события; отсутствие `set_stopped` ломает
  композицию.
- `when_all` требует разделяемого состояния между двумя operation state;
  используйте общий узел (`State`) и проверяйте, что обе ветви дошли.
- Ошибка должна **пробрасываться** дальше (`set_error`), `then` её не
  перехватывает — для восстановления нужен `upon_error`.
- В реальном `std::execution` результат — «сигнатуры завершения» и типы
  (`value_types`), а не фиксированный `int`; здесь тип значения упрощён до `int`
  ради наглядности.
- `sync_wait` блокирует вызывающий поток; в проде его место — `scheduler` и
  асинхронные приёмники.

## Задача

Реализовать мини-модель `just`, `just_error`, `then`, `upon_error`, `when_all`,
`sync_wait` (возвращает `std::expected<int, std::string>`) и собрать пайплайн:
`when_all(then(just(41), +1), upon_error(just_error, →100))` = `142`; проверить
проброс ошибки и восстановление.

## Где применяется

- Асинхронный I/O и сетевые сервисы (`m11`), планировщики задач (`m12`), агрегация
  результатов, обработка ошибок в пайплайнах, GPU-исполнение.

## Проверка

Вывод: `when_all ... = 142`, `error channel = fail`, `upon_error recovery = 10`,
availability `__cpp_lib_senders`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex04_senders_execution.cpp` (полный рабочий код). Ключевые фрагменты:

```cpp
template <class S, class G>
struct UponError {
    S sender;
    G g;
    template <class R>
    struct Op {
        struct Inner {
            G g;
            R* out;
            void set_value(int value) { out->set_value(value); }
            void set_error(std::string error) { out->set_value(g(std::move(error))); }
            void set_stopped() { out->set_stopped(); }
        };
        // ...
    };
};

template <class S>
std::expected<int, std::string> sync_wait(S sender) {
    std::optional<int> value;
    std::optional<std::string> error;
    struct R {
        std::optional<int>* value;
        std::optional<std::string>* error;
        void set_value(int v) { *value = v; }
        void set_error(std::string e) { *error = std::move(e); }
        void set_stopped() {}
    };
    auto op = sender.connect(R{&value, &error});
    op.start();
    if (error) return std::unexpected(*error);
    return *value;
}
```
