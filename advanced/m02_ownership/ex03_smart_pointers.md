# m02_ownership/ex03_smart_pointers — Умные указатели

> **Модуль:** `m02_ownership` · **Сложность:** ★★★☆☆ · **Время:** 50–70 мин
> **Запуск:** `./run.ps1 m02_ownership/ex03_smart_pointers`

## Теория (кратко)

Умные указатели выражают **владение** и освобождают ресурс в деструкторе
(RAII). Выбор указателя — это выбор модели владения.

| Указатель | Владение | Копируется | Когда |
|---|---|---|---|
| `std::unique_ptr<T>` | единоличное | нет (только move) | по умолчанию |
| `std::shared_ptr<T>` | совместное (refcount) | да | реальная общая собственность |
| `std::weak_ptr<T>` | наблюдает, не владеет | да | разрыв циклов, кэши |
| `T*` / `T&` | **не владеет** | — | параметры, наблюдатели |

## Синтаксис

```cpp
auto p = std::make_unique<Widget>(args);        // vladelec
std::unique_ptr<Widget> q = std::move(p);       // peredacha vladeniya
p.reset();                                       // osvobodit'

auto s = std::make_shared<Widget>(args);
std::weak_ptr<Widget> w = s;                     // nablyudaem
if (auto locked = w.lock()) { use(*locked); }    // zhiv li obekt

std::unique_ptr<FILE, decltype(&std::fclose)> f(std::fopen(...), &std::fclose);
```

## Подводные камни

- `shared_ptr` по умолчанию не решает циклы ссылок: `A`↔`B` через
  `shared_ptr` не освободятся. Разрывайте `weak_ptr`.
- Два `shared_ptr`, созданных из одного сырого указателя, → двойное удаление;
  всегда `make_shared`/`make_unique`.
- `unique_ptr` в параметре функции по значению означает передачу владения;
  если владение не нужно — берите `T*`/`T&`/`span`.
- Не создавайте `shared_ptr` из `this` без `enable_shared_from_this`.

## Задача

В `ex03_smart_pointers.cpp`:

1. фабрикой `make_widget` верните `std::unique_ptr<Widget>`;
2. передайте владение через `std::move` и покажите освобождение;
3. сделайте `std::shared_ptr` и продемонстрируйте `use_count()`;
4. заведите «наблюдателя» `weak_ptr` и `lock()`;
5. оберните `std::fclose` кастомным делитером для `FILE*`.

## Где применяется

- `unique_ptr` — владение сущностями, ресурсами, `pimpl`, фабрики.
- `shared_ptr` — разделяемые кэши, ресурсы, на которые есть несколько
  «владельцев» (шейдеры, текстуры в подсистемах).
- `weak_ptr` — обратные ссылки в графах сцены, не владеющие указатели на
  «родителя».

## Проверка

Прогон печатает счётчики `use_count`, факт освобождения и работу делитера;
завершается `0`.

### Решение

См. `ex03_smart_pointers.cpp`.
