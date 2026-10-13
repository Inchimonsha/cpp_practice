# m15_systems/ex05_memory_debugging — аллокаторы и детекция повреждений памяти

> Модуль m15_systems · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m15_systems/ex05_memory_debugging`
>
> В MinGW **нет** ASan/UBSan; доступны канареечные проверки, отладочный режим
> libstdc++ (`-D_GLIBCXX_ASSERTIONS`) и `g++ -fanalyzer` (см. m14).

## Теория

Большинство «загадочных» падений C++ — следствие **повреждения памяти**:
выход за границу, `use-after-free`, двойное освобождение, чтение
неинициализированного. Стратегии обнаружения:

- **канарейки (canary/guard bytes)** — магические байты вокруг блока; при
  перезаписи детектируется повреждение;
- **guard pages** — страница без прав доступа сразу после блока; выход в неё
  даёт `SIGSEGV`;
- **санитайзеры** ASan/UBSan (адреса/UB), MSan (неинициализированное);
- **собственные аллокаторы** (arena/pool) для контроля границ и производительности.

Здесь — **arena-аллокатор** (линейное выделение из общего буфера + `reset`) и
детектор канареек.

## Синтаксис

```cpp
class Arena {                       // lineynyy (bump) allokator
public:
    std::byte* allocate(std::size_t size, std::size_t alignment) {
        const std::size_t aligned = (offset_ + alignment - 1) / alignment * alignment;
        if (aligned + size > storage_.size()) return nullptr;   // otsutstvie proverki = UB
        offset_ = aligned + size;
        return storage_.data() + aligned;
    }
    void reset() { offset_ = 0; }   // massovoe osvobozhdenie za O(1)
private:
    std::vector<std::byte> storage_;
    std::size_t offset_ = 0;
};

// canary:
std::vector<unsigned char> block(kPayload + kGuard, kGuardByte);
// ... zapis' v payload ...
bool intact = std::all_of(block.begin() + kPayload, block.end(),
                          [](unsigned char b) { return b == kGuardByte; });
```

## Подводные камни

- **Выравнивание**: даже корректный по размеру указатель с неверным выравниванием
  — UB для типов с `alignof > 1`; всегда округляйте `offset_` вверх.
- **Арена не освобождает отдельные блоки**: подходит для короткоживущих
  батчей; иначе фрагментация и рост памяти.
- **Канарейки ловят только перезапись**: чтение за границей (`use-after-free`)
  незаметно — для этого guard pages/ASan.
- **Detector сам не должен падать**: проверяйте границы до чтения, не после.
- **ASan/UBSan требования**: нужны соответствующие рантаймы; в этой MinGW-сборке
  их нет (`-fsanitize=address` не линкуется), поэтому `run.ps1 -Sanitize`
  откатывается к `-g -O1 -D_GLIBCXX_ASSERTIONS` с предупреждением.
- **valgrind** под Windows не работает — используйте ASan/специфику платформы.

## Задача

Выделить из арены два блока (с выравниванием 16), проверить выравнивание и
порядок, поставить канарейки вокруг полезной нагрузки, убедиться в их
целостности, затем смоделировать перезапись и её **обнаружить**. Сбросить арену.

## Где применяется

- Отладка утечек и UB, игровые/серверные аллокаторы, real-time-пулы,
  форензика падений, контроль границ в парсерах.

## Проверка

Вывод: `arena used = 40, aligned ptr = yes`, `canary intact = true`,
`corruption detected = true`, `arena reset = 0`, `memory debugging done`.
Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex05_memory_debugging.cpp`. Обратите внимание на проверку выравнивания через
`reinterpret_cast<std::uintptr_t>(p) % 16`.
