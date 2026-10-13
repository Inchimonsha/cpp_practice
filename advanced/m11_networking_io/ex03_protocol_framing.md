# m11_networking_io/ex03_protocol_framing — фрейминг сообщений

> Модуль m11_networking_io · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m11_networking_io/ex03_protocol_framing -Extra '-lws2_32'`

## Теория

TCP — это **поток байт**, а не последовательность сообщений. Границы сообщений
протокол задаёт сам — **фрейминг**. Классические схемы:

- длина + полезная нагрузка (length-prefixed);
- разделитель (CRLF, `\0`);
- самоописывающие форматы (TLV, protobuf).

`recv` может вернуть часть фрейма, несколько фреймов сразу или один байт.
Поэтому нужен **декодер**, который накапливает байты в буфере и «выдаёт» готовые
сообщения по мере поступления.

Для length-prefixed используем 4-байтовый префикс в **сетевом порядке**
(big-endian), затем полезную нагрузку.

## Синтаксис

```cpp
std::vector<std::uint8_t> encode_frame(const std::string& payload);
// [ len:4 BE ][ payload... ]

class FrameDecoder {
    void feed(const std::uint8_t* data, std::size_t n);  // dobavit' bajty
    std::optional<std::string> next();                   // gotovyy freym?
};
```

## Подводные камни

- **Partial read**: префикс и тело могут прийти по частям; пока байт меньше
  заявленного, фрейм не готов — возвращаем `nullopt`, не «съедаем» данные.
- **Множественные фреймы** в одном `recv`: `next()` вызывается в цикле, пока не
  вернёт `nullopt`.
- **Сетевой порядок** (big-endian) через явные сдвиги; `htons`/`htonl` только для
  `uint16`/`uint32`, тогда как порядок полей — на усмотрение протокола.
- **Доверие длине**: злонамеренный префикс (например, 4 ГБ) приведёт к
  перевыделению памяти — нужен лимит максимального размера фрейма (в проде).
- **Буфер**: наивное `erase` из начала `vector` — O(n); под нагрузкой
  используют кольцевой буфер или два индекса (`read`/`write`).
- Фрейминг должен совпадать на обеих сторонах; смешение «length» и «delim»
  ломает синхронизацию потока.

## Задача

Реализовать `encode_frame` (4-байтовый big-endian префикс) и `FrameDecoder`
(`feed`/`next`). Продемонстрировать: (1) чистое декодирование двух фреймов,
поданных с разрывом (3 байта, затем остаток), — первый вызов `next()` пуст;
(2) транспорт через сокет: клиент отправляет оба фрейма одним `send`, сервер
принимает и декодирует их.

## Где применяется

- RPC и игровые протоколы, очереди сообщений, обёртки поверх TCP, бинарные
  форматы (protobuf/flatbuffers), мультиплексирование сообщений.

## Проверка

Вывод: размеры фреймов (`9`), `after 3 bytes: frame ready = false`,
`decoded (pure): hello, world`, `transport: received 2 frames` + значения,
`protocol framing done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex03_protocol_framing.cpp`. Ключевое:

```cpp
std::optional<std::string> FrameDecoder::next() {
    if (buffer_.size() < 4) return std::nullopt;
    std::uint32_t length = (buffer_[0] << 24) | (buffer_[1] << 16) |
                           (buffer_[2] << 8)  |  buffer_[3];
    if (buffer_.size() < 4 + length) return std::nullopt;
    std::string payload(reinterpret_cast<const char*>(buffer_.data() + 4), length);
    buffer_.erase(buffer_.begin(), buffer_.begin() + 4 + length);
    return payload;
}
```
