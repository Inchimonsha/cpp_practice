# m13_interop_ffi/ex02_dynamic_loading — загрузка библиотек в runtime

> Модуль m13_interop_ffi · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m13_interop_ffi/ex02_dynamic_loading`

## Теория

**Динамическая загрузка** позволяет подключать код во время работы программы:

1. Загрузить библиотеку → **дескриптор** (handle).
2. Найти символ по имени → **указатель на функцию**.
3. Вызвать.
4. Выгрузить библиотеку.

Это основа плагинов, «мягких» зависимостей (нет библиотеки — нет функции) и
версионирования без пересборки.

| Платформа | Загрузка | Символ | Выгрузка |
|---|---|---|---|
| Windows | `LoadLibraryA/W` | `GetProcAddress` | `FreeLibrary` |
| POSIX | `dlopen` | `dlsym` | `dlclose` |

Дескриптор оборачивается в RAII: деструктор обязательно вызывает
`FreeLibrary`/`dlclose`. Ошибки — через `GetLastError()` (Windows) или
`dlerror()` (POSIX).

## Синтаксис

```cpp
class SharedLibrary {
public:
    explicit SharedLibrary(const char* name) : handle_(LoadLibraryA(name)) {}
    ~SharedLibrary() { if (handle_) FreeLibrary(handle_); }

    template <class Function>
    Function symbol(const char* name) const {
        return reinterpret_cast<Function>(
            reinterpret_cast<void*>(GetProcAddress(handle_, name)));
    }
private:
    HMODULE handle_;
};

SharedLibrary lib("ucrtbase.dll");
using strlen_fn = std::size_t (*)(const char*);
auto strlen_fn_ptr = lib.symbol<strlen_fn>("strlen");
```

## Подводные камни

- **Имя функции не украшено** только у `extern "C"`/C-библиотек. Для C++-символа
  придётся указывать mangled-имя (`_Z...`) или использовать `extern "C"`-фасад.
- **`LoadLibrary` ищет по порядку** PATH/каталогов и по имени с расширением;
  не указывать путь — зависит от окружения. Кладите рядом с exe или давайте
  абсолютный путь.
- **Локальная библиотека тянет свои зависимости**; при несовпадении рантаймов
  (разные CRT) — отказ загрузки или порча кучи.
- **Не выгружайте, пока живы указатели** на её функции (иначе вызов в
  освобождённую память). Освобождайте в обратном порядке захвата.
- **Ошибки различайте**: модуль не найден (`126`), символ не найден
  (`GetProcAddress` → `NULL`). Проверяйте оба случая.
- **Кэширование**: повторная `LoadLibrary` возвращает тот же handle и
  увеличивает счётчик ссылок — парная `FreeLibrary` обязательна.

## Задача

Загрузить `ucrtbase.dll`, найти `strlen`, вызвать её для строки
`"hello, ffi"`. Показать три случая: успешный символ, отсутствующий символ
(`nullptr`), отсутствующий модуль (ошибка `126`). Вывести результат.

## Где применяется

- Плагины (редакторы, движки, браузеры), ленивая линковка, драйверы,
  кроссплатформенные бэкенды (load `libc`/`ws2_32`), тестовые подмены.

## Проверка

Вывод: `loaded: ucrtbase.dll`, `strlen("hello, ffi") = 10`,
`missing symbol -> nullptr`, `missing library -> nullptr (error 126)`,
`dynamic loading done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex02_dynamic_loading.cpp`. RAII-класс `SharedLibrary` не копируется и не
перемещается, чтобы дескриптор не выгрузился дважды.
