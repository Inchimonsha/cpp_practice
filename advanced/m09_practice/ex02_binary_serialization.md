# ex02_binary_serialization — бинарная сериализация и версии формата

## Теория

Бинарная сериализация превращает объект в поток байтов. Чтобы формат был
переносимым и расширяемым:

- **Фиксированный порядок байтов** (endianness): на проводе обычно
  little-endian; конвертируйте `std::endian::native`. `std::byteswap` (C++23)
  меняет порядок.
- **Заголовок**: magic-число, версия, при необходимости длина/флаги.
- **Явные типы фиксированной ширины**: `std::uint32_t`, не `int`/`long`.
- **Версионирование**: новые поля добавляйте в конец; старый ридер читает
  префикс. При несовместимости — увеличивайте major.
- `std::span` даёт невладеющий вид на буфер, `std::bit_cast` — безопасную
  переинтерпретацию битов фиксированных типов.

## Синтаксис

```cpp
std::vector<std::byte> out;
put(out, std::uint32_t{42});          // 4 bajta LE

std::span<const std::byte> data = out;
std::size_t offset = 0;
std::uint32_t value = 0;
get(data, offset, value);

if (std::endian::native == std::endian::big) { /* swap */ }
```

## Подводные камни

- `reinterpret_cast` структуры в байты непереносим (padding, endianness) —
  сериализуйте поля по отдельности.
- `std::bit_cast` требует одинакового размера и тривиальной копируемости; не
  применяйте к типам с указателями/vtable.
- Строки: храните длину отдельно; `std::string` как «сырой» буфер — не ABI.
- Проверяйте вход: короткий/битый буфер не должен приводить к чтению за границей
  (используйте `span` и проверку смещений).
- float через `bit_cast<uint32_t>` сохраняет точные биты, но не текст.

## Задача

Сериализовать запись `Player{id:7, x:1.5, y:-2.25, hp:100, name:"neo"}`:
заголовок `"SV"` + версия `1`, затем поля little-endian, затем длина имени (1
байт) и байты. Десериализовать обратно и сверить. Показать ошибку при неверной
версии. Размер буфера — `23` байта.

## Где применяется

- Форматы сохранений, сетевые пакеты, кэши, межъязыковой обмен (в т.ч. с Lua/C),
  память GPU.

## Проверка

Вывод: размер буфера, восстановленные поля, ошибка версии; код возврата `0` при
совпадении, иначе `1`.

## Решение

```cpp
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <iostream>
#include <print>
#include <span>
#include <string>
#include <vector>

namespace {

constexpr std::uint16_t kMagic = 0x5356;   // 'S','V'
constexpr std::uint8_t kVersion = 1;

struct Player {
    std::uint32_t id;
    float x;
    float y;
    std::int32_t hp;
    std::string name;
};

template <class T>
void put(std::vector<std::byte>& out, T value) {
    auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    if constexpr (std::endian::native == std::endian::big) {
        std::reverse(bytes.begin(), bytes.end());
    }
    out.insert(out.end(), bytes.begin(), bytes.end());
}

template <class T>
bool get(std::span<const std::byte> data, std::size_t& offset, T& out) {
    if (offset + sizeof(T) > data.size()) { return false; }
    std::array<std::byte, sizeof(T)> bytes{};
    std::copy_n(data.begin() + static_cast<std::ptrdiff_t>(offset), sizeof(T),
                bytes.begin());
    if constexpr (std::endian::native == std::endian::big) {
        std::reverse(bytes.begin(), bytes.end());
    }
    out = std::bit_cast<T>(bytes);
    offset += sizeof(T);
    return true;
}

std::vector<std::byte> serialize(const Player& player) {
    std::vector<std::byte> out;
    put(out, kMagic);
    put(out, kVersion);
    put(out, player.id);
    put(out, std::bit_cast<std::uint32_t>(player.x));
    put(out, std::bit_cast<std::uint32_t>(player.y));
    put(out, player.hp);
    put(out, static_cast<std::uint8_t>(player.name.size()));
    for (char ch : player.name) {
        out.push_back(static_cast<std::byte>(ch));
    }
    return out;
}

std::expected<Player, std::string> deserialize(std::span<const std::byte> data) {
    std::size_t offset = 0;
    std::uint16_t magic = 0;
    std::uint8_t version = 0;
    Player player{};
    std::uint32_t x_bits = 0;
    std::uint32_t y_bits = 0;
    std::uint8_t name_len = 0;

    if (!get(data, offset, magic) || magic != kMagic) {
        return std::unexpected("bad magic");
    }
    if (!get(data, offset, version) || version != kVersion) {
        return std::unexpected("unsupported version");
    }
    if (!get(data, offset, player.id) || !get(data, offset, x_bits) ||
        !get(data, offset, y_bits) || !get(data, offset, player.hp) ||
        !get(data, offset, name_len)) {
        return std::unexpected("truncated buffer");
    }
    if (offset + name_len > data.size()) {
        return std::unexpected("truncated name");
    }
    player.x = std::bit_cast<float>(x_bits);
    player.y = std::bit_cast<float>(y_bits);
    player.name.assign(reinterpret_cast<const char*>(data.data() + offset), name_len);
    return player;
}

}  // namespace

int main() {
    const Player original{7, 1.5f, -2.25f, 100, "neo"};
    const std::vector<std::byte> buffer = serialize(original);
    std::println("buffer size = {} bytes", buffer.size());
    std::println("native endian = {}",
                 std::endian::native == std::endian::little ? "little" : "big");

    auto restored = deserialize(buffer);
    if (!restored) {
        std::println("deserialize failed: {}", restored.error());
        return 1;
    }
    std::println("id={} x={} y={} hp={} name={}",
                 restored->id, restored->x, restored->y,
                 restored->hp, restored->name);

    std::vector<std::byte> bad = buffer;
    bad[2] = static_cast<std::byte>(99);   // podmena versii
    auto broken = deserialize(bad);
    if (!broken) {
        std::println("version check: {}", broken.error());
    }

    const bool ok = restored->id == original.id && restored->x == original.x &&
                    restored->y == original.y && restored->hp == original.hp &&
                    restored->name == original.name && buffer.size() == 24 &&
                    !broken;
    std::cout << "binary serialization done\n";
    return ok ? 0 : 1;
}
```
