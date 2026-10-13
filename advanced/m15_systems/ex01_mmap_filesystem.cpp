// m15_systems/ex01_mmap_filesystem — fayl otobrazhen v pamyat' (memory-mapped file).
// Windows: CreateFileMapping/MapViewOfFile. POSIX analog: mmap()/open()/ftruncate().
// Determinizovano. Standart: C++23. Zapusk: ./run.ps1 m15_systems/ex01_mmap_filesystem
#include <windows.h>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

namespace {

constexpr std::size_t kProbeOffset = 12345;
constexpr unsigned char kNewValue = 99;

void fill_pattern(std::vector<unsigned char>& data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<unsigned char>((i * 31 + 7) % 256);
    }
}

}  // namespace

int main() {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "cpp_course_mmap.bin";
    constexpr std::size_t kSize = 1024 * 1024;

    std::vector<unsigned char> expected(kSize);
    fill_pattern(expected);

    {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(expected.data()),
                  static_cast<std::streamsize>(expected.size()));
    }

    const std::uintmax_t file_size = std::filesystem::file_size(path);
    std::println("file size = {}", file_size);

    HANDLE file = CreateFileW(path.wstring().c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        std::println("CreateFile failed (error {})", GetLastError());
        return 1;
    }

    HANDLE mapping = CreateFileMappingW(file, nullptr, PAGE_READWRITE, 0, 0, nullptr);
    if (mapping == nullptr) {
        std::println("CreateFileMapping failed (error {})", GetLastError());
        CloseHandle(file);
        return 1;
    }

    auto* view = static_cast<unsigned char*>(MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    if (view == nullptr) {
        std::println("MapViewOfFile failed (error {})", GetLastError());
        CloseHandle(mapping);
        CloseHandle(file);
        return 1;
    }
    std::println("mapped view = ok");

    const unsigned char old_value = view[kProbeOffset];
    const bool match = old_value == expected[kProbeOffset];
    view[kProbeOffset] = kNewValue;
    std::println("modified offset {}: old = {}, new = {}", kProbeOffset, old_value, kNewValue);

    UnmapViewOfFile(view);
    CloseHandle(mapping);
    CloseHandle(file);

    unsigned char on_disk = 0;
    {
        std::ifstream in(path, std::ios::binary);
        in.seekg(static_cast<std::streamoff>(kProbeOffset));
        in.read(reinterpret_cast<char*>(&on_disk), 1);
    }
    std::println("verified on disk = {}", on_disk);

    std::error_code ignored;
    std::filesystem::remove(path, ignored);

    const bool ok = file_size == kSize && match && on_disk == kNewValue;
    std::cout << "mmap filesystem done\n";
    return ok ? 0 : 1;
}
