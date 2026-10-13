// m16_projects/ex01_kv_store — in-memory khranilishche "klyuch-znachenie" s
// sokhraneniem na disk (snapshot) i zagruzkoy.
// Determinizovano. Standart: C++23. Zapusk: ./run.ps1 m16_projects/ex01_kv_store
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>

namespace {

class KeyValueStore {
public:
    void put(std::string key, std::string value) {
        entries_[std::move(key)] = std::move(value);
    }

    std::optional<std::string> get(std::string_view key) const {
        const auto it = entries_.find(std::string(key));
        if (it == entries_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    bool erase(std::string_view key) { return entries_.erase(std::string(key)) > 0; }

    std::size_t size() const { return entries_.size(); }

    bool save(const std::filesystem::path& path) const {
        std::ofstream out(path, std::ios::trunc);
        if (!out) {
            return false;
        }
        for (const auto& [key, value] : entries_) {
            out << key << '=' << value << '\n';
        }
        return static_cast<bool>(out);
    }

    bool load(const std::filesystem::path& path) {
        std::ifstream in(path);
        if (!in) {
            return false;
        }
        entries_.clear();
        std::string line;
        while (std::getline(in, line)) {
            const std::size_t separator = line.find('=');
            if (separator == std::string::npos) {
                continue;
            }
            entries_[line.substr(0, separator)] = line.substr(separator + 1);
        }
        return true;
    }

private:
    std::unordered_map<std::string, std::string> entries_;
};

}  // namespace

int main() {
    KeyValueStore store;
    store.put("a", "1");
    store.put("b", "2");
    store.put("c", "3");
    std::println("initial size = {}", store.size());

    const bool had_b = store.get("b").has_value();
    const bool erased = store.erase("b");
    std::println("erase b: had = {}, erased = {}", had_b, erased ? "yes" : "no");
    std::println("get b after erase = {}", store.get("b").has_value() ? "value" : "missing");

    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "cpp_course_kv_store.db";
    const bool saved = store.save(path);
    std::println("saved = {}, persisted entries = {}", saved ? "yes" : "no", store.size());

    KeyValueStore restored;
    const bool loaded = restored.load(path);
    const std::string a = restored.get("a").value_or("-");
    const std::string c = restored.get("c").value_or("-");
    std::println("restored: a = {}, c = {}, size = {}, b missing = {}", a, c, restored.size(),
                 restored.get("b").has_value() ? "no" : "yes");

    std::error_code ignored;
    std::filesystem::remove(path, ignored);

    const bool ok = store.size() == 2 && had_b && erased && saved && loaded && a == "1" &&
                    c == "3" && restored.size() == 2 && !restored.get("b").has_value();
    std::cout << "kv store done\n";
    return ok ? 0 : 1;
}
