# m11_networking_io/ex05_http_server — минимальный HTTP/1.1-сервер

> Модуль m11_networking_io · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m11_networking_io/ex05_http_server -Extra '-lws2_32'`

## Теория

HTTP/1.1 — текстовый протокол «запрос-ответ» поверх TCP:

```
GET /hello HTTP/1.1\r\n
Host: localhost\r\n
Connection: close\r\n
\r\n
```

- **Стартовая строка**: метод, путь (target), версия.
- **Заголовки**: `Имя: значение` до пустой строки.
- **Тело** — после пустой строки; наличие задаёт `Content-Length`.

Ответ: строка статуса (`HTTP/1.1 200 OK`), заголовки (как минимум
`Content-Length`), пустая строка, тело. Сервер читает запрос до `\r\n\r\n`,
маршрутизирует по `method`+`path` и формирует ответ. Заголовок `Connection:
close` упрощает жизнь: одно соединение — один запрос.

## Синтаксис

```cpp
struct HttpRequest { std::string method, path, version;
                     std::map<std::string, std::string> headers; };
std::optional<HttpRequest> parse_request(const std::string& raw);

std::string build_response(int status, const std::string& reason,
                           const std::string& content_type, const std::string& body) {
    std::string r = std::format("HTTP/1.1 {} {}\r\n", status, reason);
    response += std::format("Content-Type: {}\r\n", content_type);
    response += std::format("Content-Length: {}\r\n", body.size());
    response += "Connection: close\r\n\r\n";
    response += body;
    return response;
}
```

## Подводные камни

- **Читать до `\r\n\r\n`**, а не один `recv`: заголовки могут прийти частями;
  тело (если есть) читается по `Content-Length`.
- **Регистр и пробелы**: имена заголовков регистронезависимы, значения обрезают
  пробелы; строка запроса разбирается по пробелам (`method`, `path`, `version`).
- **Content-Length** обязателен в ответе (иначе клиент не знает границу тела);
  `Connection: close` упрощает, но реальные серверы держат keep-alive.
- **Безопасность**: не подставляйте `path` в файловую систему без нормализации
  (path traversal); ограничивайте размер запроса и заголовков.
- Валидация: `parse_request` возвращает `nullopt` на «мусор» — не впадайте в
  обработку частично разобранного запроса.
- HTTP — регистронезависим для метода, но чувствителен к пути; маршрутизацию
  делайте по нормализованному пути.

## Задача

Реализовать `parse_request` (строка запроса + заголовки), `build_response`
(статус, `Content-Type`, `Content-Length`, `Connection: close`) и маршруты: `GET /`
→ `200 root`, `GET /hello` → `200 Hello, HTTP!`, иначе `404 not found`. Сервер
обслуживает два запроса (`/hello`, `/missing`) через сокет, клиент печатает
код и тело.

## Где применяется

- REST API, статические серверы, health-check эндпоинты, тестовые заглушки,
  микросервисы, встроенные админ-панели.

## Проверка

Вывод: `GET /hello -> 200`, `body: Hello, HTTP!`, `GET /missing -> 404`,
`body: not found`, `http server done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex05_http_server.cpp`. Ключевое:

```cpp
std::optional<HttpRequest> parse_request(const std::string& raw) {
    std::size_t line_end = raw.find("\r\n");
    if (line_end == std::string::npos) return std::nullopt;
    HttpRequest request;
    std::istringstream stream(raw.substr(0, line_end));
    if (!(stream >> request.method >> request.path >> request.version)) return std::nullopt;
    // ... zagolovki do pustoy stroki ...
    return request;
}

std::string route(const HttpRequest& request) {
    if (request.method == "GET" && request.path == "/hello")
        return build_response(200, "OK", "text/plain", "Hello, HTTP!");
    return build_response(404, "Not Found", "text/plain", "not found");
}
```
