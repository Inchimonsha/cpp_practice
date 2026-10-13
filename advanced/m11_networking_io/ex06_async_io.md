# m11_networking_io/ex06_async_io — композиция асинхронных операций над сокетами

> Модуль m11_networking_io · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m11_networking_io/ex06_async_io -Extra '-lws2_32'`

## Теория

Асинхронный I/O состоит не из одного `recv`, а из **композиции операций**:
«прочитать из сокета A и из сокета B, объединить результаты», «прочитать, затем
преобразовать», «прочитать или по таймауту — ошибка».

Модель **sender/receiver** (см. `m10_language_future/ex04`,
`m06_concurrency/ex06`) описывает такие операции лениво:

- `read_sender{fd, timeout}` — операция чтения; при `start()` делает неблокирующее
  ожидание (`select`) и вызывает `set_value(std::string)` или
  `set_error(std::string)`.
- `then(sender, f)` — преобразование результата.
- `when_all(s1, s2, f)` — дождаться обеих операций и объединить.
- `sync_wait(sender)` — «драйвер»: запускает операцию и забирает результат в
  `std::expected<std::string, std::string>`.

В реальном `std::execution` драйвер — это scheduler (пул потоков, `io_context`);
здесь он синхронный, но композиция та же.

## Синтаксис

```cpp
auto op = async::then(async::read_sender{fd, 2000}, to_upper);
auto result = async::sync_wait(op);            // expected<string,string>

auto both = async::when_all(async::read_sender{a, 2000},
                            async::read_sender{b, 2000},
                            [](std::string x, std::string y) { return x + "+" + y; });
```

## Подводные камни

- Операции **ленивы**: без `connect`+`start` (или `sync_wait`) ничего не
  происходит.
- `when_all` держит общее состояние; объединяйте только когда **обе** ветви
  завершились, и пробрасывайте ошибку немедленно.
- `then` не перехватывает ошибки — для восстановления нужен `upon_error`
  (см. `m10/ex04`).
- Сокет владеет RAII-обёртка: не храните `socket_t` после уничтожения `Socket`.
- `read_sender` делает **один** `recv`; для полноты сообщения нужен фрейминг
  (`ex03`).
- Блокирующий `sync_wait` удобен в примерах, но в проде операции исполняет
  планировщик, а не поток-инициатор.

## Задача

Реализовать `read_sender`, `then`, `when_all`, `sync_wait`. Продемонстрировать
на loopback: (1) `then(read, to_upper)` → `HELLO`; (2) `when_all` двух чтений →
`world+boom`; (3) ошибка при закрытии соединения сервером → `closed`.

## Где применяется

- Высоконагруженные серверы (одна операция — не один поток), прокси, клиенты
  API, агрегация ответов от нескольких источников, тайм-ауты как first-class.

## Проверка

Вывод: `then(read_sender) = HELLO`, `when_all = world+boom`, `error = closed`,
`async io done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex06_async_io.cpp`. Ключевое:

```cpp
struct read_sender {
    socket_t fd;
    int timeout_ms;
    template <class Receiver>
    struct operation {
        socket_t fd; int timeout_ms; Receiver receiver;
        void start() {
            if (!poll_readable(fd, timeout_ms)) { receiver.set_error("timeout"); return; }
            char buffer[128] = {};
            int count = recv(fd, buffer, sizeof(buffer), 0);
            if (count <= 0) { receiver.set_error("closed"); return; }
            receiver.set_value(std::string(buffer, (std::size_t)count));
        }
    };
    template <class Receiver>
    operation<Receiver> connect(Receiver receiver) const {
        return operation<Receiver>{fd, timeout_ms, std::move(receiver)};
    }
};

const auto result = async::sync_wait(
    async::when_all(async::read_sender{second.get(), 2000},
                    async::read_sender{third.get(), 2000},
                    [](std::string a, std::string b) { return a + "+" + b; }));
```
