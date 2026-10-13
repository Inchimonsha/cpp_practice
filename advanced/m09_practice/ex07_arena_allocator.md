# ex07_arena_allocator — bump/arena-аллокатор и std::pmr

## Теория

Arena (bump) аллокатор выделяет память из большого буфера простым сдвигом
указателя:

- Выделение — O(1): выровнять смещение, сдвинуть его на размер.
- Освобождение — коллективное: `reset()` сбрасывает смещение, возвращая **всю**
  арену сразу. Индивидуального `free` нет.
- Идеален для данных с одинаковым временем жизни: кадр рендера, парсинг запроса,
  компиляция.

Стандартная версия — `std::pmr::monotonic_buffer_resource`: отдаёт память из
заданного буфера, при исчерпании — из вышестоящего ресурса. Контейнеры
`std::pmr::vector` принимают такой ресурс и перестают дергать глобальный `new`.

## Синтаксис

```cpp
std::align(alignment, size, ptr, space);        // <memory>

std::array<std::byte, 1024> buffer;
std::pmr::monotonic_buffer_resource res(buffer.data(), buffer.size());
std::pmr::vector<int> values(&res);              // <memory_resource>

T* p = std::construct_at(reinterpret_cast<T*>(mem), args...);   // <memory>
```

## Подводные камни

- Выравнивание обязательно: без `std::align` объекты с `alignof > 1` получат
  невыровненный адрес (UB на многих платформах).
- `reset()` не вызывает деструкторы — для нетривиальных типов вызывайте их явно
  (`std::destroy_at`) до сброса.
- Память живёт, пока жив буфер, переданный ресурсу; не давайте ресурсу пережить
  буфер.
- Арена — не замена общего аллокатора: рост вразнобой/освобождение вразнобой
  тратят память впустую.
- Не путайте `monotonic_buffer_resource` (только вперёд) с
  `unsynchronized_pool_resource` (переиспользование блоков).

## Задача

Реализовать `Arena` с `allocate(size, alignment)` (через `std::align`),
`create<T>(args...)`, `reset()`, `used()`. Разместить `int`, `double` и `Vec2`,
проверить выравнивание и объём. Затем выделить `std::pmr::vector<int>` из
`monotonic_buffer_resource` поверх буфера и убедиться, что данные лежат внутри
буфера.

## Где применяется

- Игровые движки (per-frame/level arenas), парсеры и компиляторы, веб-серверы
  (per-request), аллокаторы контейнеров (`std::pmr`), встраиваемые системы.

## Проверка

Вывод: занятые байты `4 → 16 → 24`, обе проверки выравнивания, сумма pmr-вектора
`4950` и признак «внутри буфера»; код возврата `0`, иначе `1`.

## Решение

```cpp
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <memory_resource>
#include <new>
#include <print>
#include <utility>
#include <vector>

namespace {

struct Vec2 { float x; float y; };

class Arena {
public:
    explicit Arena(std::size_t capacity)
        : buffer_(std::make_unique<std::byte[]>(capacity)), capacity_(capacity) {}

    void* allocate(std::size_t size,
                   std::size_t alignment = alignof(std::max_align_t)) {
        void* ptr = buffer_.get() + offset_;
        std::size_t space = capacity_ - offset_;
        void* aligned = std::align(alignment, size, ptr, space);
        if (aligned == nullptr) { throw std::bad_alloc(); }
        offset_ = static_cast<std::size_t>(
                      static_cast<std::byte*>(aligned) - buffer_.get()) + size;
        return aligned;
    }

    template <class T, class... Args>
    T* create(Args&&... args) {
        return std::construct_at(reinterpret_cast<T*>(allocate(sizeof(T), alignof(T))),
                                 std::forward<Args>(args)...);
    }

    void reset() { offset_ = 0; }
    std::size_t used() const { return offset_; }
    std::size_t capacity() const { return capacity_; }

private:
    std::unique_ptr<std::byte[]> buffer_;
    std::size_t capacity_;
    std::size_t offset_ = 0;
};

bool aligned_as(const void* ptr, std::size_t alignment) {
    return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

}  // namespace

int main() {
    Arena arena(256);
    std::println("capacity = {}", arena.capacity());

    int* number = arena.create<int>(42);
    std::println("after int: used = {}", arena.used());

    double* ratio = arena.create<double>(3.5);
    std::println("after double: used = {}", arena.used());

    Vec2* point = arena.create<Vec2>(1.0f, 2.0f);
    std::println("after Vec2: used = {}", arena.used());

    std::println("int aligned = {}", aligned_as(number, alignof(int)));
    std::println("double aligned = {}", aligned_as(ratio, alignof(double)));
    std::println("value = {} {} ({}, {})", *number, *ratio, point->x, point->y);

    const std::size_t used_before_reset = arena.used();
    arena.reset();
    std::println("after reset: used = {}", arena.used());

    std::array<std::byte, 1024> buffer{};
    std::pmr::monotonic_buffer_resource resource(buffer.data(), buffer.size());
    std::pmr::vector<int> values(&resource);
    long long sum = 0;
    for (int i = 0; i < 100; ++i) {
        values.push_back(i);
        sum += i;
    }
    const auto* first = reinterpret_cast<const std::byte*>(values.data());
    const bool in_buffer = first >= buffer.data() && first < buffer.data() + buffer.size();
    std::println("pmr sum = {}, in buffer = {}", sum, in_buffer);

    const bool ok = used_before_reset == 24 &&
                    aligned_as(number, alignof(int)) &&
                    aligned_as(ratio, alignof(double)) &&
                    arena.used() == 0 && sum == 4950 && in_buffer;
    std::cout << "arena allocator done\n";
    return ok ? 0 : 1;
}
```
