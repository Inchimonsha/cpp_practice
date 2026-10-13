# m11_networking_io/ex01_sockets — TCP-сокеты и RAII

> Модуль m11_networking_io · Сложность ★★ · C++23 · Запуск: `./run.ps1 m11_networking_io/ex01_sockets -Extra '-lws2_32'`

## Теория

**Сокет** — дескриптор конечной точки связи. Потоковый TCP (`SOCK_STREAM`)
даёт надёжный упорядоченный поток байт: `socket → bind → listen → accept`
на сервере и `socket → connect` на клиенте, далее `send`/`recv`.

Сокеты — **системный ресурс** (файловый дескриптор). Его нужно детерминированно
освобождать, поэтому оборачиваем в RAII:

- `WsaGuard` — инициализация/деинициализация Winsock (`WSAStartup`/`WSACleanup`),
  аналог `getaddrinfo`-инициализации; на POSIX пустой.
- `Socket` — владеет дескриптором, запрещает копирование, поддерживает move,
  закрывает в деструкторе (`closesocket`/`close`).

Порт `0` при `bind` означает «любой свободный»; фактический номер узнаём через
`getsockname` — удобно для тестов, чтобы не занимать фиксированный порт.

## Синтаксис

```cpp
Socket server(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
sockaddr_in addr{}; addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
addr.sin_port = 0;
bind(server.get(), (sockaddr*)&addr, sizeof addr);
socklen_type len = sizeof addr;
getsockname(server.get(), (sockaddr*)&addr, &len);   // uznat' port
listen(server.get(), 4);
// ...
sockaddr_in peer{...}; peer.sin_port = htons(port);
connect(client.get(), (sockaddr*)&peer, sizeof peer);
send(client.get(), data, n, 0);
recv(client.get(), buf, sizeof buf, 0);
```

## Подводные камни

- Платформенные различия: на Windows дескриптор — `SOCKET`, закрытие —
  `closesocket`, инициализация — `WSAStartup`; на POSIX — `int` и `close`.
  Прячьте их за `#ifdef _WIN32`.
- `<winsock2.h>` подключайте **до** `<windows.h>` и определяйте
  `WIN32_LEAN_AND_MEAN` (иначе конфликт старых `winsock.h`/`winsock2.h`).
- `send`/`recv` — **не** гарантируют полную передачу: возможны короткие
  операции (partial read/write); в общем случае нужен цикл.
- `recv` возвращает `0` при закрытии соединения и `<0` при ошибке — обрабатывайте
  отдельно от «данные пришли».
- На MinGW линкуйте `-lws2_32`; `run.ps1` пробрасывает это через
  `-Extra '-lws2_32'`.
- Порядок байт: сетевой порядок (big-endian) — `htons`/`htonl`/`ntohs`.
- `bind`/`connect` на «занятый» порт падают — используйте эфемерный порт в тестах.

## Задача

Реализовать RAII-обёртки `WsaGuard`/`Socket`, сервер (`bind` на `127.0.0.1:0`,
`getsockname`, `listen`) и клиент (`connect` по полученному порту). Сервер в
отдельном `std::jthread` принимает соединение, читает `"ping"` и эхо-возвращает
его. Проверить, что сервер получил `ping` и клиент получил эхо.

## Где применяется

- Клиент-серверные приложения, микросервисы, игровой сетевой транспорт, RPC,
  прокси, а также любой сетевой протокол уровнем выше TCP.

## Проверка

Вывод: строка про прослушивание, `client -> server: "ping"`,
`server received: "ping"`, `client <- server: "ping"`. Код возврата `0` при
совпадении, иначе `1`. Порт не печатается (эфемерный) — вывод детерминирован.

## Решение

См. `ex01_sockets.cpp`. Ключевые фрагменты:

```cpp
Socket listen_loopback(std::uint16_t& port) {
    Socket server = make_tcp();
    sockaddr_in address{};
    address.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(server.get(), reinterpret_cast<sockaddr*>(&address), sizeof(address));
    socklen_type length = sizeof(address);
    getsockname(server.get(), reinterpret_cast<sockaddr*>(&address), &length);
    port = ntohs(address.sin_port);
    listen(server.get(), 4);
    return server;
}

std::jthread server([&] {
    Socket connection(accept(listener.get(), nullptr, nullptr));
    char buffer[128] = {};
    int n = recv(connection.get(), buffer, sizeof(buffer), 0);
    send(connection.get(), buffer, n, 0);
});
```
