# m11_networking_io/ex04_timeouts_cancellation — таймауты и отмена

> Модуль m11_networking_io · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m11_networking_io/ex04_timeouts_cancellation -Extra '-lws2_32'`

## Теория

Сетевой код нельзя писать как «вечный блокирующий `recv`»: нужно уметь
**ждать с таймаутом** и **отменять** работу.

- **Таймаут**: `select` (или `poll`/`epoll`) с `timeval` ждёт готовности не дольше
  заданного времени. Возврат `0` — таймаут; `>0` — есть готовые дескрипторы;
  `<0` — ошибка.
- **Отмена**: `std::jthread` + `std::stop_token` — кооперативная остановка. Поток
  сам проверяет `token.stop_requested()` в точках паузы; «зависший» блокирующий
  вызов отменить нельзя, поэтому цикл строят из коротких ожиданий
  (`select(..., 50ms)`) и проверяют токен между ними.

Такой цикл — основа «мягкого» завершения сервисов: `request_stop()`, затем
`join()`.

## Синтаксис

```cpp
bool poll_readable(socket_t fd, int timeout_ms) {
    fd_set read_fds; FD_ZERO(&read_fds); FD_SET(fd, &read_fds);
    timeval tv{timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    int ready = select((int)fd + 1, &read_fds, nullptr, nullptr, &tv);
    return ready > 0 && FD_ISSET(fd, &read_fds);
}

std::jthread worker([&](std::stop_token token) {
    while (!token.stop_requested()) {
        (void)poll_readable(fd, 50);        // korotkoe ozhidanie
    }
});
worker.request_stop();
worker.join();
```

## Подводные камни

- Блокирующий `recv` **не** прерывается `stop_token` — отмена возможна только
  через неблокирующий режим или периодический `select`.
- Таймаут `select` **видоизменяет** `timeval` на некоторых системах — не
  переиспользуйте структуру без переинициализации.
- Различайте таймаут (`ready == 0`) и ошибку (`ready < 0`): ошибку нельзя считать
  «просто нет данных».
- «Проснуться и прочитать» ≠ «прочитать всё»: после готовности всё равно нужен
  цикл на полное сообщение (см. `ex03`).
- Небольшие таймауты (десятки мс) дают отзывчивую отмену, но нагружают CPU;
  крайности вредны в обе стороны.
- `std::jthread` вызывает `request_stop()` в деструкторе и джойнится — не
  забывайте, что деструктор может блокироваться до завершения потока.

## Задача

Реализовать `poll_readable`. Продемонстрировать: (1) таймаут — до отправки
данных чтение «не готово» (`false` за 150 мс); (2) готовность после `send`;
(3) отмену — `std::jthread` крутит цикл с коротким `select`, главный поток делает
`request_stop()`, поток корректно выходит по `stop_token`.

## Где применяется

- Тайм-ауты RPC/HTTP, heartbeat, реконнекты, graceful shutdown серверов,
  watchdog-и, управление долгими операциями.

## Проверка

Вывод: `timeout (150 ms): readable = false`, `after send: readable = true`,
`received: "data"`, `worker: stop_token observed, loop exited`,
`timeouts/cancellation done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex04_timeouts_cancellation.cpp`. Ключевое:

```cpp
const bool ready_before = poll_readable(connection.get(), 150);   // false
send(client.get(), message.data(), (int)message.size(), 0);
const bool ready_after = poll_readable(connection.get(), 2000);   // true

std::jthread worker([&](std::stop_token token) {
    while (!token.stop_requested()) {
        (void)poll_readable(connection.get(), 50);
    }
});
worker.request_stop();
worker.join();
```
