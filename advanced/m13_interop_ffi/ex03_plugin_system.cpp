// m13_interop_ffi/ex03_plugin_system — ABI-plaginy, versii i zhiznennyy cikl.
// Plaginy sozdayutsya v processe; real'naya zagruzka DLL ispol'zuet tot zhe
// ABI-cherez LoadLibraryA + GetProcAddress("create_..."), sm. ex02.
// Standart: C++23. Zapusk: ./run.ps1 m13_interop_ffi/ex03_plugin_system
#include <cstdint>
#include <iostream>
#include <print>
#include <vector>

extern "C" {

struct PluginApi {
    std::uint32_t abi_version;
    const char* name;
    int (*evaluate)(int, int);
    void (*destroy)(PluginApi*);
};

PluginApi* create_add_plugin() {
    auto* api = new PluginApi{};
    api->abi_version = 1;
    api->name = "add";
    api->evaluate = +[](int left, int right) { return left + right; };
    api->destroy = +[](PluginApi* self) { delete self; };
    return api;
}

PluginApi* create_mul_plugin() {
    auto* api = new PluginApi{};
    api->abi_version = 1;
    api->name = "mul";
    api->evaluate = +[](int left, int right) { return left * right; };
    api->destroy = +[](PluginApi* self) { delete self; };
    return api;
}

PluginApi* create_bad_plugin() {
    auto* api = new PluginApi{};
    api->abi_version = 99;
    api->name = "bad";
    api->evaluate = +[](int, int) { return 0; };
    api->destroy = +[](PluginApi* self) { delete self; };
    return api;
}
}

class PluginHost {
public:
    static constexpr std::uint32_t abi_version = 1;

    ~PluginHost() {
        for (PluginApi* plugin : plugins_) {
            if (plugin->destroy != nullptr) {
                plugin->destroy(plugin);
            }
        }
    }

    bool load(PluginApi* plugin) {
        if (plugin == nullptr || plugin->abi_version != abi_version) {
            if (plugin != nullptr && plugin->destroy != nullptr) {
                plugin->destroy(plugin);
            }
            return false;
        }
        plugins_.push_back(plugin);
        return true;
    }

    const std::vector<PluginApi*>& plugins() const { return plugins_; }

private:
    std::vector<PluginApi*> plugins_;
};

int main() {
    PluginHost host;
    host.load(create_add_plugin());
    host.load(create_mul_plugin());
    const bool rejected = !host.load(create_bad_plugin());

    std::println("host abi version = {}", PluginHost::abi_version);
    for (const PluginApi* plugin : host.plugins()) {
        std::println("plugin {}: 3, 4 -> {}", plugin->name, plugin->evaluate(3, 4));
    }
    std::println("rejected bad-abi plugin = {}", rejected);

    bool ok = rejected && host.plugins().size() == 2;
    ok = ok && host.plugins()[0]->evaluate(3, 4) == 7;
    ok = ok && host.plugins()[1]->evaluate(3, 4) == 12;
    std::cout << "plugin system done\n";
    return ok ? 0 : 1;
}
