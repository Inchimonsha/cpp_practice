# ex04_profiling — профилирование горячих путей

## Теория

Профилирование отвечает на вопрос «где программа реально тратит время/память»,
в отличие от догадок. Основные виды:

- **Семплирующие профилировщики** (`perf`, VTune, `gprof` c `-pg`) —
  периодически снимают стек; дают распределение по функциям и флеймграфы.
- **Инструментирующие** — считают вызовы/время по каждой функции точно, но
  замедляют программу.
- **Обратные**: для памяти (Massif, heaptrack), для промахов кэша (perf stat),
  для ветвлений.

Методика: собрать с символами (`-g`), не терять оптимизации (`-O2`), прогнать
реалистичную нагрузку, найти верхние «горячие» функции, оптимизировать их,
профилировать заново.

## Синтаксис (инструменты MinGW/Linux)

```bash
# Linux: sempling
perf record -g ./app && perf report

# gprof (GNU): instrumentirovanie
g++ -O2 -pg app.cpp -o app && ./app && gprof ./app gmon.out
```

## Подводные камни

- Профилирование без оптимизаций (`-O0`) искажает картину: узкие места
  «переезжают».
- Разные прогоны → разный профиль; нужна репрезентативная нагрузка и несколько
  запусков.
- Символы обязательны (`-g`), иначе в отчёте адреса вместо имён.
- Микрооптимизация не-горячей функции не даёт эффекта (правило 90/10).
- Внешние профилировщики зависят от ОС; в этом курсе основной акцент — измерение
  и правильная постановка эксперимента.

## Задача

Написать «намеренно наивное» приложение: подсчёт простых чисел до `N` пробным
делением (горячий цикл `is_prime`). Вывести: количество простых до `100000`
(должно быть `9592`), затраченное время, и подсказку, как профилировать
(`-pg` + `gprof`, либо `perf`). Проверка — по количеству простых.

## Где применяется

- Поиск узких мест, подготовка к оптимизации, контроль регрессий, разбор
  потребления памяти.

## Проверка

Вывод: `count = 9592`, время, подсказка; код возврата `0` при `count == 9592`,
иначе `1`.

## Решение

```cpp
#include <chrono>
#include <cstdint>
#include <iostream>
#include <print>

namespace {
bool is_prime(int n) {
    if (n < 2) { return false; }
    if (n % 2 == 0) { return n == 2; }
    for (int d = 3; static_cast<std::int64_t>(d) * d <= n; d += 2) {
        if (n % d == 0) { return false; }
    }
    return true;
}

int count_primes(int limit) {
    int count = 0;
    for (int n = 2; n <= limit; ++n) {
        if (is_prime(n)) { ++count; }
    }
    return count;
}
}  // namespace

int main() {
    constexpr int kLimit = 100000;

    const auto t0 = std::chrono::steady_clock::now();
    const int count = count_primes(kLimit);
    const auto t1 = std::chrono::steady_clock::now();
    const double ms =
        std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::println("primes up to {} = {}", kLimit, count);
    std::println("elapsed = {:.2f} ms (mashinno-zavisimoe)", ms);
    std::println("profile hint: g++ -O2 -g -pg ... && gprof; ili: perf record -g");

    std::cout << "profiling done\n";
    return count == 9592 ? 0 : 1;
}
```
