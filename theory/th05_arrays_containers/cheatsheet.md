# Cheatsheet: th05 — массивы и контейнеры

## std::array — фиксированный размер (стек)

```cpp
std::array<int, 3> a{1, 2, 3};
a.size();          // 3
a.at(9);           // throw std::out_of_range
a[0];              // без проверки
```

## std::vector — растущий (куча)

```cpp
std::vector<int> v;
v.reserve(100);          // ёмкость заранее
v.push_back(1);          // копия
v.emplace_back(2);       // конструирование на месте
v.size();                // количество
v.capacity();            // ёмкость (>= size)
v.at(i);                 // с проверкой (исключение)
v[i];                    // без проверки (UB за границей)
v.erase(v.begin() + i);  // сдвиг элементов
```

## Итераторы и range-for

```cpp
for (int i = 0; i < (int)v.size(); ++i) { v[i]; }
for (auto it = v.begin(); it != v.end(); ++it) { *it; }
for (const auto& x : v) { /* чтение */ }
for (auto& x : v) { /* изменение */ }
```

## std::span (C++20) — окно над данными

```cpp
void f(std::span<const int> s) { for (int x : s) { /* */ } }

f(arr3);       // std::array
f(vec);        // std::vector
f(c_arr, 3);   // C-массив
```

## Ловушки

```cpp
v.size() - 1;   // при пустом векторе — огромное число
auto& x = v[0]; // ссылка «умрёт» при перераспределении
for (auto& x : v) { v.push_back(x); }  // НЕЛЬЗЯ: инвалидация итератора
```