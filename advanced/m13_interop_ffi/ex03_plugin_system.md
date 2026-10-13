# m13_interop_ffi/ex03_plugin_system — ABI плагинов, версии и жизненный цикл

> Модуль m13_interop_ffi · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m13_interop_ffi/ex03_plugin_system`

## Теория

Плагин — это код, который **хост** загружает во время работы. Контракт задаёт
**C-ABI-структура** с версией, именем и указателями на функции:

```cpp
struct PluginApi {
    std::uint32_t abi_version;      // proverka sovmestimosti
    const char* name;
    int (*evaluate)(int, int);
    void (*destroy)(PluginApi*);    // vozvrat' vladeniya hostu
};
```

Правила надёжной архитектуры плагинов:

- **Версия ABI**. Хост отвергает плагины с несовпадающей версией — иначе
  вызов через неверно понятую структуру = падение.
- **Фабрика**. Библиотека экспортирует `create_plugin()`, возвращающую
  структуру; хост получает её через `GetProcAddress` (см. `ex02`).
- **Жизненный цикл**. Кто создал — тот уничтожает; `destroy` реализует плагин,
  хост лишь вызывает.
- **Отсутствие исключений через границу**: C-ABI не знает C++-исключений;
  ошибки — кодами возврата или особыми значениями.

## Синтаксис

```cpp
extern "C" PluginApi* create_add_plugin() {
    auto* api = new PluginApi{};
    api->abi_version = 1;
    api->name = "add";
    api->evaluate = +[](int a, int b) { return a + b; };   // bezraxvatnyy lambda -> fptr
    api->destroy = +[](PluginApi* self) { delete self; };
    return api;
}

class PluginHost {
public:
    static constexpr std::uint32_t abi_version = 1;
    bool load(PluginApi* plugin);        // proveryaet versiyu
    ~PluginHost();                       // vyzyvaet destroy() na vsekh
private:
    std::vector<PluginApi*> plugins_;
};
```

## Подводные камни

- **Утечка при отвержении**: если версия не подошла, хост всё равно должен
  вызвать `destroy` (или сам `delete`), иначе утечка.
- **Кто владеет памятью**: `new` в плагине и `delete` в хосте через разные CRT
  — опасно; поэтому `destroy` вызывает **сам плагин**.
- **Несовпадение языка ABI**: `bool`, `enum`, размеры типов должны быть
  зафиксированы (`std::int32_t`), иначе плагин, собранный другим компилятором,
  «поедет».
- **Порядок полей** в структуре фиксирован — вставка поля в середину ломает
  ABI (поэтому и нужна версия).
- **Потокобезопасность**: реестр плагинов при загрузке из нескольких потоков
  требует синхронизации.
- Дублирующий символ при статической линковке: несколько плагинов не должны
  экспортировать одно и то же имя.

## Задача

Определить ABI `PluginApi` с версией, реализовать плагины `add`/`mul` (версия
1) и «плохой» плагин (версия 99). Хост загружает совместимые, отвергает
несовместимый, вызывает `evaluate(3,4)` у загруженных и освобождает ресурсы.

## Где применяется

- Редакторы и IDE, графические пакеты, серверы приложений, антивирусы,
  браузерные расширения, игровые движки (мод-loading).

## Проверка

Вывод: `host abi version = 1`, `plugin add: 3, 4 -> 7`,
`plugin mul: 3, 4 -> 12`, `rejected bad-abi plugin = true`,
`plugin system done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex03_plugin_system.cpp`. Реальная загрузка DLL использует тот же ABI через
`LoadLibraryA` + `GetProcAddress("create_add_plugin")` (см. `ex02`).
