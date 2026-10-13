# Cheatsheet: th03 — функции и области видимости

## Способы передачи параметров

| Синтаксис | Копия | Мутация? | Когда |
|---|---|---|---|
| `void f(int x)` | да | нет | мелкие значения |
| `void f(int& x)` | нет | да | результат наружу |
| `void f(const int& x)` | нет | нет | большие объекты, чтение |
| `void f(int* p)` | нет | через `*p` | nullable |

## Синтаксис функций

```cpp
// объявление (чаще в .hpp):
int damage(int base, int mult);
int scale(int v, int factor = 2);     // умолчание — в объявлении, справа

// определение (в .cpp):
int damage(int base, int mult) { return base * mult; }

// перегрузка — разные сигнатуры:
int scale(int v) { return v; }
double scale(double v) { return v * 1.5; }

// inline — маленькие функции в заголовке:
inline int sq(int x) { return x * x; }
```

## Области видимости

```cpp
namespace {              // анонимный namespace: видимо внутри .cpp
    constexpr int kPrivate = 42;
    int helper() { return kPrivate; }
}

void f(int x) {          // x — параметр: виден в теле
    int local = x;       // блочная видимость
}

static int counter() {   // static в функции: одна копия, живёт вечно
    static int n = 0;
    return ++n;
}
```

## Правила курса

- Значение — для мелких; `const&` — для больших; `&` — только «результат наружу».
- Чистые функции — без скрытого состояния.
- Глобальных изменяемых переменных — нет.
- `.hpp` — объявления, `.cpp` — определения, `#pragma once` в начале заголовка.