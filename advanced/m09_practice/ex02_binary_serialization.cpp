// m09_practice/ex02_binary_serialization — binarnaya serializaciya i versii formata.
// Standart: C++23. Zapusk: ./run.ps1 m09_practice/ex02_binary_serialization
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
                    restored->name == original.name && buffer.size() == 23 &&
                    !broken;
    std::cout << "binary serialization done\n";
    return ok ? 0 : 1;
}
