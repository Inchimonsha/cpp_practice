// m02_ownership/ex01_raii — RAII i scope guard.
// Standart: C++23. Zapusk: ./run.ps1 m02_ownership/ex01_raii
#include <cstdio>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace {

// RAII-obnyortka nad std::FILE*.
class File {
public:
    explicit File(const char* path) : handle_(std::fopen(path, "w+")) {
        if (!handle_) { throw std::runtime_error("fopen failed"); }
        std::println("  File: opened");
    }
    ~File() {
        if (handle_) {
            std::fclose(handle_);
            std::println("  File: closed");
        }
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    void write(std::string_view text) {
        std::fwrite(text.data(), 1, text.size(), handle_);
    }

private:
    std::FILE* handle_{};
};

// Scope guard: vypolnit' deystvie pri vyhode iz oblasti.
class ScopeGuard {
public:
    explicit ScopeGuard(std::string name) : name_(std::move(name)) {}
    ~ScopeGuard() { std::println("  guard: {}", name_); }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    std::string name_;
};

void inner() {
    ScopeGuard a{"inner-a"};
    ScopeGuard b{"inner-b"};
    throw std::runtime_error("boom");  // obe guard srabotayut pri raskrutke
}

}  // namespace

int main() {
    const char* path = "ex01_raii_demo.tmp";
    {
        File file{path};
        file.write("hello, RAII");
    }  // zdes' fajl zakryt
    std::remove(path);

    std::println("LIFO order:");
    {
        ScopeGuard outer{"outer"};
        ScopeGuard inner{"inner"};  // razrushen perвым
        (void)inner;
    }

    std::println("cleanup on exception:");
    try {
        inner();
    } catch (const std::exception& e) {
        std::println("  caught: {}", e.what());
    }

    std::cout << "raii done\n";
    return 0;
}
