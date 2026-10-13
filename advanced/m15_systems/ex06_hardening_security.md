# m15_systems/ex06_hardening_security — безопасность и hardening

> Модуль m15_systems · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m15_systems/ex06_hardening_security`
>
> Реальные флаги (недоступны/иные в MinGW): `-D_FORTIFY_SOURCE=2`,
> `-fstack-protector-strong`, ASLR/DEP, `/GUARD:CF`, CFI.

## Теория

**Hardening** — набор приёмов, снижающих эксплуатируемость дефектов:

- **безопасные границы**: `std::span`, проверка длины до копирования; не
  использовать `strcpy`/`sprintf`/`gets`;
- **защита от целочисленного переполнения**: `__builtin_add_overflow` или
  `std::add_overflow` (C++26) вместо `a + b > limit` (само выражение уже UB);
- **затирание секретов**: компилятор может удалить «мёртвую» очистку — нужен
  `volatile`/`std::memset_s`/`SecureZeroMemory`/`explicit_bzero`;
- **компиляторные защиты**: stack canary (`-fstack-protector-strong`),
  `-D_FORTIFY_SOURCE=2` (проверяемые версии `memcpy` и др.), PIE/ASLR,
  non-executable stack (NX/DEP), CFI/`/GUARD:CF`;
- **принцип наименьших привилегий**, sandboxing, валидация всех входов.

## Синтаксис

```cpp
std::size_t bounded_copy(std::span<char> dst, std::string_view src) {
    const std::size_t n = std::min(dst.size(), src.size());   // granica do kopirovaniya
    std::copy_n(src.begin(), n, dst.begin());
    return n;
}

std::size_t sum = 0;
bool overflow = __builtin_add_overflow(big, std::size_t{10}, &sum);   // ne UB

void secure_erase(void* data, std::size_t size) {        // ne dat' optimizirovat'
    volatile unsigned char* bytes = static_cast<volatile unsigned char*>(data);
    for (std::size_t i = 0; i < size; ++i) bytes[i] = 0;
}
```

Сборка с защитами (типично; часть флагов — на GCC/Clang):

```bash
g++ -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE -pie -Wl,-z,relro,-z,now ...
```

## Подводные камни

- **Проверка «после» сложения — уже поздно**: `if (a + b > SIZE_MAX)` само
  вызывает UB. Считайте перенос заранее (`__builtin_add_overflow`).
- **Оптимизация удаляет затирание**: без `volatile`/`memset_s` компилятор вправе
  убрать запись нулей (буфер больше не читается).
- **`std::span` не проверяет границы сам** при `operator[]`; для защищённого
  доступа — `at()`/ручные проверки; span лишь несёт размер.
- **`_FORTIFY_SOURCE` требует оптимизации** (`-O2` и выше) — при `-O0`
  проверки могут отсутствовать.
- **Канарейка стека не ловит перезапись кучи**; нужны кучевые защиты
  (allocator hardening) и ASan.
- **Нельзя полагаться только на hardening**: устраняйте дефект, а не маскируйте
  его защитой. Проверяйте входные данные на границе доверия.

## Задача

Реализовать безопасное копирование через `std::span` (с ограничением длины),
детектировать целочисленное переполнение `__builtin_add_overflow`, затереть
секрет через `volatile` и убедиться в отсутствии «хвоста».

## Где применяется

- Сетевые сервисы, парсеры недоверенных данных, криптография (затирание ключей),
  браузеры/песочницы, встроенные устройства с моделью угроз.

## Проверка

Вывод: `bounded copy: copied 5, dst = "hello"`,
`integer overflow detected = true`, `secure erase verified = true`,
`span size = 3`, `hardening security done`. Код возврата `0` при совпадении,
иначе `1`.

## Решение

См. `ex06_hardening_security.cpp`. Обратите внимание: проверка переполнения
происходит *до* записи результата, а `secure_erase` использует `volatile`.
