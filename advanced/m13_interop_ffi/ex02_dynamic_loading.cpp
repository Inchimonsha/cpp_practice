// m13_interop_ffi/ex02_dynamic_loading — zagruzka bibliotek v runtime (LoadLibraryA).
// Standart: C++23 (Win32). Zapusk: ./run.ps1 m13_interop_ffi/ex02_dynamic_loading
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cstddef>
#include <iostream>
#include <print>

class SharedLibrary {
public:
    explicit SharedLibrary(const char* name) : handle_(LoadLibraryA(name)) {}
    ~SharedLibrary() {
        if (handle_ != nullptr) {
            FreeLibrary(handle_);
        }
    }
    SharedLibrary(const SharedLibrary&) = delete;
    SharedLibrary& operator=(const SharedLibrary&) = delete;
    SharedLibrary(SharedLibrary&&) = delete;
    SharedLibrary& operator=(SharedLibrary&&) = delete;

    bool ok() const { return handle_ != nullptr; }

    template <class Function>
    Function symbol(const char* name) const {
        return reinterpret_cast<Function>(
            reinterpret_cast<void*>(GetProcAddress(handle_, name)));
    }

private:
    HMODULE handle_;
};

int main() {
    SharedLibrary library("ucrtbase.dll");
    if (!library.ok()) {
        std::println("failed to load ucrtbase.dll (error {})", GetLastError());
        return 1;
    }
    std::println("loaded: ucrtbase.dll");

    using string_length_function = std::size_t (*)(const char*);
    const string_length_function string_length = library.symbol<string_length_function>("strlen");
    const int length = string_length != nullptr
                           ? static_cast<int>(string_length("hello, ffi"))
                           : -1;
    std::println("strlen(\"hello, ffi\") = {}", length);

    const auto missing_symbol = library.symbol<void*>("definitely_missing_symbol");
    std::println("missing symbol -> {}", missing_symbol == nullptr ? "nullptr" : "found");

    SharedLibrary missing("definitely_missing_module_xyz.dll");
    const bool missing_ok = !missing.ok();
    const unsigned long missing_error = missing_ok ? GetLastError() : 0UL;
    std::println("missing library -> nullptr (error {})", missing_error);

    const bool ok = length == 10 && missing_symbol == nullptr && missing_ok;
    std::cout << "dynamic loading done\n";
    return ok ? 0 : 1;
}
