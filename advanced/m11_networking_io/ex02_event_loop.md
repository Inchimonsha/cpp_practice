# m11_networking_io/ex02_event_loop — реактор на `select`

> Модуль m11_networking_io · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m11_networking_io/ex02_event_loop -Extra '-lws2_32'`

## Теория

**Событийный цикл (event loop)** мультиплексирует много дескрипторов в одном
потоке: вместо «поток на соединение» один цикл спрашивает у ОС, какие сокеты
**готовы** к чтению/записи, и обрабатывает только готовые. Классический примитив
— `select` (есть и в Winsock, и в POSIX); его старшие братья — `poll`, `epoll`
(Linux), `kqueue` (BSD/macOS), IOCP (Windows).

**Реактор (reactor)** — структура, которая регистрирует дескрипторы и вызывает
обработчики по событиям готовности. Это основа большинства event-loop библиотек
(libuv, asio в режиме реактора, Redis, Node.js).

## Синтаксис

```cpp
fd_set read_fds;
FD_ZERO(&read_fds);
FD_SET(listener.get(), &read_fds);
FD_SET(connection.get(), &read_fds);
socket_t max_fd = std::max(listener.get(), connection.get());

timeval timeout{1, 0};                        // 1 sekunda
int ready = select((int)max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
if (ready > 0) {
    if (FD_ISSET(listener.get(), &read_fds)) { /* accept */ }
    if (FD_ISSET(connection.get(), &read_fds)) { /* recv */ }
}
```

## Подводные камни

- `select` **изменяет** наборы `fd_set` (оставляет только готовые), поэтому их
  нужно пересобирать на каждой итерации.
- Первый аргумент `select` (`nfds = max_fd + 1`) значим только на POSIX; Windows
  его игнорирует, но передавать корректное значение переносимо.
- `FD_SETSIZE` ограничивает число дескрипторов (обычно 64 на Windows) — для
  масштаба используют `poll`/`epoll`/`IOCP`.
- Готовность к чтению ≠ «данные есть»: `recv` может вернуть `0` (EOF) или
  `<0` (ошибка) — обрабатывайте отдельно.
- События коалесцируются: несколько сообщений могут прийти одним `recv`; нельзя
  полагаться на «одно событие — одно сообщение».
- `select` с ненулевым таймаутом выходит по таймауту (`0`) — не путайте с ошибкой
  (`<0`).
- На Windows `max_fd` типа `SOCKET` (беззнаковый) — не полагайтесь на сравнение
  со знаком.

## Задача

Построить мини-реактор: зарегистрировать слушающий сокет и (после `accept`)
соединение; в цикле `select` обрабатывать готовность: `accept` нового соединения,
`recv` данных до EOF. Клиент в отдельном `std::jthread` шлёт три строки
(`msg1..msg3`) и закрывается. Собрать принятый поток и вывести построчно.

## Где применяется

- Сетевые серверы (прокси, чаты, игровые лобби), драйверы ввода, обработка
  множества таймеров/дескрипторов в одном потоке, ядро асинхронных библиотек.

## Проверка

Вывод: `reactor: accepted connection`, три строки `msg1`/`msg2`/`msg3`,
`event loop done`. Код возврата `0` при трёх сообщениях, иначе `1`.

## Решение

См. `ex02_event_loop.cpp`. Ключевой цикл:

```cpp
while (!done && idle_ticks < 100) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(listener.get(), &read_fds);
    socket_t max_fd = listener.get();
    if (connection.get() != k_invalid) {
        FD_SET(connection.get(), &read_fds);
        if (connection.get() > max_fd) max_fd = connection.get();
    }
    timeval timeout{1, 0};
    if (select((int)max_fd + 1, &read_fds, nullptr, nullptr, &timeout) <= 0) {
        ++idle_ticks;
        continue;
    }
    if (FD_ISSET(listener.get(), &read_fds)) {
        Socket peer(accept(listener.get(), nullptr, nullptr));
        if (peer.get() != k_invalid) connection = std::move(peer);
    }
    if (connection.get() != k_invalid && FD_ISSET(connection.get(), &read_fds)) {
        char buffer[128] = {};
        int count = recv(connection.get(), buffer, sizeof(buffer), 0);
        if (count <= 0) done = true;
        else received.append(buffer, (std::size_t)count);
    }
}
```
