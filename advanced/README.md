# Advanced C++ 2026 — курс: синтаксис и практика

Продвинутый курс по современному C++ (стандарт **C++23**, с обзором
экспериментальных возможностей **C++26**) на реальных задачах, фрагментах кода и
проектах. Это углублённое продолжение основного курса (`../README.md`), но
проходится как самостоятельная программа.

> Язык материалов — русский; имена API, типов и паттернов — в оригинале с
> пояснением при первом упоминании.

---

## 1. Цели и принципы

1. **От синтаксиса к инженерии.** Сначала — полное повторение синтаксиса языка
   (модуль `m01_syntax`), затем — владение ресурсами, обобщённое
   программирование, STL, конкурентность, производительность.
2. **Практика на реальном коде.** Каждое задание — это работающий `.cpp` на
   узнаваемой задаче (парсер, пул потоков, ECS, аллокатор, рендер), а не
   синтетический «примерчик».
3. **Краткая теория + синтаксис + задача.** Каждый файл `exNN_*.md` содержит
   краткую теорию, блок синтаксиса, постановку, подсказки и раздел «где это
   применяется в реальном коде».
4. **Быстрый запуск.** Один файл — один запуск: `./run.ps1 m01_syntax/ex01_types_literals`.
5. **Проверяемость.** Все примеры собираются GCC 16.2 (`ucrt64`) без
   предупреждений; поведение — детерминированное.

## 2. Формат курса и запуск (выбранный для C++)

**Формат:** *single-file* — каждое задание это один самостоятельный `.cpp` с
`int main()`, который компилируется напрямую без CMake и внешних зависимостей.
Ряд заданий, где нужны несколько единиц трансляции или чужая библиотека, имеет
подпапку с `CMakeLists.txt` (явно помечено в задании).

**Именование и группировка** (как требует план): `module_name/exNN_title`.

```
advanced/
├── README.md                 ← этот файл: план курса и трекер прогресса
├── run.ps1 / run.cmd / run.sh← компиляция и запуск одного файла
├── .build/                   ← артефакты запуска (в .gitignore)
├── m00_tooling/
│   ├── ex01_workflow.md
│   └── ex01_workflow.cpp
├── m01_syntax/
│   ├── ex01_types_literals.md
│   └── ex01_types_literals.cpp
└── ...
```

**Запуск** (можно из папки `advanced/` или из корня репозитория):

```powershell
# из папки advanced/
./run.ps1 m01_syntax/ex01_types_literals      # путь к .cpp или к заданию
./run.ps1 m01_syntax/ex01_types_literals.cpp  # можно с расширением
./run.ps1 m02_ownership/ex04_move_semantics -Std c++26 -Sanitize
./run.ps1 m00_tooling/ex02_compiler_flags -NoRun   # только собрать

# из корня репозитория
advanced\run.cmd m00_tooling/ex01_workflow
```

Скрипт сам находит `g++` (в т.ч. `C:\msys64\ucrt64\bin\g++.exe`), добавляет его
каталог в `PATH` (иначе `cc1plus.exe` не стартует), собирает в `advanced/.build/`
и запускает.

**Флаги компиляции по умолчанию:** `-std=c++23 -O2 -Wall -Wextra -Wpedantic -Wshadow -lstdc++exp`.
Флаг `-lstdc++exp` нужен на MinGW-GCC для линковки `std::print`, `std::println`
и `std::stacktrace`; на Linux он также доступен. Прочие флаги — в заданиях.

> **Совет.** Для регулярной проверки качества включите
> `-Werror -Wconversion -Wsign-conversion` в отдельном прогоне: это ловит
> неявные сужения (`m00_tooling/ex02_compiler_flags`). В обычном режиме они
> выключены, чтобы не мешать экспериментам.

> **Ограничение MinGW.** AddressSanitizer/UBSan в MinGW-GCC не поставляются
> (`cannot find -lasan`). Поэтому `-Sanitize` на Windows-MinGW включает проверки
> STL (`_GLIBCXX_ASSERTIONS`) и предупреждает; полноценные ASan/UBSan доступны
> на Linux (или через WSL, `run.sh`) и в clang/MSVC. Учитывайте это при работе с
> заданиями `m00_tooling/ex03_debug_sanitizers` и `m07_performance`.

## 3. Окружение

| Компонент | Значение |
|---|---|
| Компилятор | MSYS2 `ucrt64` **GCC 16.2.0** (проверено в этом репозитории) |
| Стандарт | **C++23** (основной), **C++26** — раздел «preview» |
| Платформа | Windows (win32), скрипты также работают на Linux/macOS |
| Зависимости | нет (всё — стандартная библиотека); отдельные задания — CMake |

Проверено на GCC 16.2: `std::format`, `std::expected`, `std::ranges`,
`std::views::enumerate`, `std::flat_map`, `std::ranges::to<std::vector>()`,
`std::mdspan`, `std::print` (с `-lstdc++exp`).

## 4. Шаблон файла задания

Каждое задание состоит из двух файлов: `exNN_title.md` (теория + задача) и
`exNN_title.cpp` (каркас с `main` и ответами). Рекомендуемая структура `.md`:

```
# module/exNN_title — короткое название
> Модуль · Сложность ★ · Время · Запуск одной строкой

## Теория (кратко)        — 5–15 строк, зачем это нужно
## Синтаксис              — минимальный компилируемый фрагмент с пояснениями
## Подводные камни        — типичные ошибки
## Задача                 — что сделать в exNN_title.cpp
## Где применяется        — реальные библиотеки/движки/сценарии
## Проверка               — что должно получиться на экране / в тестах
```

Скелет `.cpp`:

```cpp
// module/exNN_title — <название>. Стандарт: C++23. Запуск: ./run.ps1 <путь>
#include <iostream>

int main() {
    // TODO: <постановка>
    return 0;
}
```

## 5. Карта курса (что изучим)

```
m00 tooling ─▶ m01 syntax ─▶ m02 ownership ─▶ m03 templates ─┐
                                                            ▼
      m09 practice ◀── m08 build ◀── m07 performance ◀── m06 concurrency ◀┘
           │                    m04 stl ──▶ m05 errors ──▶ (сюда)
           ▼
      capstone: мини-движок (m09)

   ── профессиональный уровень (планируется) ──
      m10 future ─▶ m11 networking ─▶ m12 parallel/gpu ─▶ m13 interop
                                                              │
      m16 projects ◀── m15 systems ◀── m14 quality ◀──────────┘
```

- `m00` — инструменты: single-file workflow, флаги, санитайзеры, CMake, тесты.
- `m01` — **полный обзор синтаксиса**: типы, инициализация, операторы, управление,
  функции, области видимости, указатели/ссылки, массивы/строки, структуры/enum,
  классы, наследование, лямбды, шаблоны (введение), исключения, C++20/23/26.
- `m02` — владение: RAII, правило 0/3/5, умные указатели, move-семантика.
- `m03` — обобщённое программирование: шаблоны, traits, SFINAE, concepts, folds.
- `m04` — STL на глубину: контейнеры, итераторы, алгоритмы, ranges, allocators/PMR.
- `m05` — обработка ошибок: исключения, гарантии, `noexcept`, `expected`.
- `m06` — многопоточность: потоки, мьютексы, атомики, task/senders, корутины.
- `m07` — производительность: модель памяти, cache, бенчмарки, профилирование, SIMD.
- `m08` — сборка и распространение: модули, CMake, ABI, упаковка, тестирование.
- `m09` — практика: реальные задачи и итоговый capstone.

   Профессиональный уровень (разделы `m10`–`m16` ниже) **спроектирован, но ещё не
   написан** — задания помечены `[ ]` в трекере (раздел 7).

- `m10` — язык завтрашнего дня: контракты, рефлексия, pattern matching, senders, `std::simd`.
- `m11` — сеть и ввод-вывод: сокеты, event loop, протоколы, таймауты, HTTP.
- `m12` — параллелизм и GPU: parallel STL, графы задач, SIMD/автовекторизация, offload.
- `m13` — совместимость и FFI: C ABI, плагины, WebAssembly, Python, Rust.
- `m14` — качество: property-based тесты, фаззинг, покрытие, статанализ, CI.
- `m15` — системное программирование: mmap/FS, процессы и IPC, real-time, embedded, безопасность.
- `m16` — прикладные проекты: серия capstone (БД, компилятор, сервис, профилировщик, плагины, рендер).

---

## 6. Модули и задания

### m00_tooling — Инструменты и рабочий процесс

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_workflow` | `main`, `std::format`, `__cplusplus`, `__VERSION__` | Единый контекст сборки; вывод окружения, как в логах CI |
| `ex02_compiler_flags` | preprocessor, `[[nodiscard]]`, `[[maybe_unused]]` | Дисциплина предупреждений; ловля сужений и `-Wshadow` |
| `ex03_debug_sanitizers` | UB, raw pointer misuse под ASan/UBSan | Отладка крашей и утечек в игровых/сетевых подсистемах |
| `ex04_cmake_multi` | разделение `.hpp`/`.cpp`, target'ы | Многофайловый проект; `INTERFACE`-библиотека флагов |
| `ex05_testing_doctest` | assertions, минимальный test-runner | Регрессии логики без интерфейса |

### m01_syntax — Полный обзор синтаксиса C++ (повторение основ)

Это ядро «пройдись по всему синтаксису»: от литералов до C++26-preview.

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_types_literals` | фундаментальные типы, размеры, пределы, суффиксы литералов, `char8_t`, raw strings | Сериализация, бинарные форматы, парсинг конфигов |
| `ex02_decl_init_const` | `auto`, формы инициализации `{}`/`()`/`=`, `const`/`constexpr`/`consteval`/`constinit`, `static`, `inline`, `thread_local` | Таблицы констант, compile-time данные |
| `ex03_operators` | все операторы, приоритет, `?:`, `,`, побитовые, `<=>`; перегрузка | Математика векторов, флаги, компареры |
| `ex04_control_flow` | `if`/`switch` с init-statement, `[[fallthrough]]`, `for`/range-for, `while`/`do`, `break`/`continue`, `goto` | Игровой цикл, циклы обработки сущностей |
| `ex05_functions` | overloads, default-аргументы, `noexcept`, `[[nodiscard]]`, trailing return, variadic (C-стиль) | Публичные API, интерфейсы движка |
| `ex06_scope_namespaces` | области видимости, `static`-локальные, namespace, `using`, ADL, linkage (`extern`/`internal`) | Организация крупного кода, избегание ODR-ошибок |
| `ex07_pointers_refs` | указатели, ссылки, `const`-квалификация, арифметика указателей, `nullptr`, pointer-to-member | Низкоуровневые структуры, interop с C |
| `ex08_arrays_strings` | C-массивы, `std::array`, `std::string`, `std::string_view`, `std::span` | Парсинг, буферы, ноль-копийные интерфейсы |
| `ex09_structs_enums_unions` | агрегаты, designated initializers, `enum class`, битовые поля, `union` | Конфиги, протоколы, теги типов |
| `ex10_classes` | конструкторы/деструктор, списки инициализации, делегирование, `explicit`, `friend`, перегрузка операторов | Модели предметной области |
| `ex11_inheritance_polymorphism` | `virtual`/`override`/`final`, абстрактные классы, множественное наследование, `dynamic_cast` | Плагины, интерфейсы подсистем |
| `ex12_lambdas` | синтаксис, захваты, init-capture, `mutable`, generic/template lambda, `constexpr` | Колбэки, компараторы, отложенные вычисления |
| `ex13_templates_intro` | function/class templates, deduction, specialization, `if constexpr`, variadic (введение) | Контейнеры, утилиты |
| `ex14_exceptions_basics` | `throw`/`try`/`catch`, catch-by-ref, unwinding, `std::exception` | Границы подсистем, восстановление после ошибок |
| `ex15_cpp20_syntax` | concepts, ranges, `std::span`, `std::format`, designated init, `using enum`, `constinit` | Новый код курса — везде |
| `ex16_cpp23_syntax` | `std::expected`, `flat_map`, `mdspan`, `std::print`, `views::enumerate`, `ranges::to`, `byteswap`, `if consteval` | Парсеры, обработка данных, вывод без `iostream` |
| `ex17_cpp26_preview` | static reflection, contracts, senders/receivers, `std::execution` (эксперим.) | Обзор трендов; проверяется по возможностям компилятора |

### m02_ownership — Владение ресурсами и перемещение

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_raii` | RAII, scope guard, деструктор как освобождение | Файлы, сокеты, локи, GPU-хендлы |
| `ex02_special_members` | правило 0/3/5, `= default`/`delete`, copy-swap | Управление ресурсом в классе |
| `ex03_smart_pointers` | `unique_ptr`/`shared_ptr`/`weak_ptr`, deleter, `make_*` | Графы ресурсов, кэши, владение в ECS |
| `ex04_move_semantics` | rvalue-ссылки, `std::move`, move-ctor/assign | Контейнеры, передача больших объектов |
| `ex05_perfect_forwarding` | forwarding references, `std::forward`, reference collapsing | Фабрики, `make_unique`-подобные API |
| `ex06_copy_elision` | RVO/NRVO, value categories (lvalue/xvalue/prvalue) | Возврат больших значений без копий |

### m03_templates — Обобщённое программирование

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_template_basics` | deduction, CTAD, NTTP, deduction guides | Утилиты без дублирования кода |
| `ex02_specialization_sfinae` | full/partial specialization, `enable_if`, tag dispatch | Перегрузка по типам, старые библиотеки |
| `ex03_type_traits` | `std::type_traits`, custom traits, detection idiom | Валидация интерфейсов на этапе компиляции |
| `ex04_concepts` | `concept`, `requires`, constraints, subsumption | Читаемые ошибки, ограниченные шаблоны |
| `ex05_variadic_folds` | parameter packs, fold expressions | Логирование, кортежи, `emplace` |
| `ex06_compile_time` | `constexpr`/`consteval`, `if constexpr`, `static_assert` | Генерация таблиц, проверки инвариантов |
| `ex07_crtp_type_erasure` | CRTP, mixins, type erasure | Полиморфизм без vtable, `std::function`-стиль |

### m04_stl — Стандартная библиотека на глубину

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_sequence_containers` | `vector`/`deque`/`list`/`forward_list`, реаллокация, capacity | Хранение сущностей, буферы |
| `ex02_associative_containers` | `map`/`unordered_map`/`set`, хеши, порядок | Индексы, кэши, реестры ресурсов |
| `ex03_adaptors_heaps` | `stack`/`queue`/`priority_queue`, heap-алгоритмы | Планировщики, A*, очереди событий |
| `ex04_iterators` | категории, `advance`/`next`, свой итератор | Обход структур, адаптеры |
| `ex05_algorithms_ranges` | `sort`/`find`/`transform`/`accumulate`, проекции, `std::ranges` | Пакетная обработка данных |
| `ex06_views_composition` | `views::filter/transform/take/enumerate`, ленивость | Конвейеры обработки без аллокаций |
| `ex07_allocators_pmr` | allocators, `std::pmr`, арены/пулы | Игровые кадровые аллокаторы |
| `ex08_strings_charconv` | `string_view`, `from_chars`/`to_chars`, `format` | Парсинг чисел, конфиги, логи |
| `ex09_chrono` | `duration`/`time_point`, clocks, календарь C++20 | Таймеры, профайлинг, игровое время |
| `ex10_functional` | `function`, `bind`, `reference_wrapper`, HOF | Колбэки, подписки |
| `ex11_sum_product_types` | `optional`/`variant`/`any`/`tuple` | Опциональные поля, тегированные значения |

### m05_errors — Обработка ошибок

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_exceptions_safety` | гарантии basic/strong/nothrow, unwinding | Транзакционность операций, парсинг |
| `ex02_noexcept` | `noexcept`, move при росте контейнера | Контейнеры, производительность |
| `ex03_expected_monadic` | `std::expected`, `.and_then`/`.transform`, `error_code` | API без исключений, парсеры |
| `ex04_error_strategies` | стратегии, логирование, контракты (preview) | Выбор модели ошибок в проекте |

### m06_concurrency — Многопоточность и конкурентность

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_threads_jthread` | `thread`, `jthread`, `stop_token` | Рабочие потоки, отмена задач |
| `ex02_mutex_locks_cv` | `mutex`, `lock_guard`, `unique_lock`, `scoped_lock`, `condition_variable` | Очереди задач, синхронизация |
| `ex03_atomics_memory_order` | `atomic`, `memory_order`, fences, модель памяти | Флаги, счётчики, lock-free |
| `ex04_async_futures` | `async`, `future`/`promise`, `packaged_task` | Асинхронные загрузки |
| `ex05_coroutines` | `co_await`/`co_yield`/`co_return`, generator/Task | Ленивые последовательности, асинхронный I/O |
| `ex06_execution_senders` | `std::execution`, senders/receivers (C++26) | Обзор планировщиков нового поколения |

### m07_performance — Производительность, память, диагностика

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_object_model_alignment` | `alignas`/`alignof`, padding, `bit_cast`, strict aliasing | Бинарные форматы, сетевые пакеты |
| `ex02_cache_data_oriented` | AoS vs SoA, locality, `noexcept`, hot/cold | ECS, системы частиц |
| `ex03_benchmarking` | `chrono`, `DoNotOptimize`, статистика замеров | Сравнение реализаций |
| `ex04_profiling` | профилировщики, флеймграфы, счётчики | Поиск «горячих» мест |
| `ex05_optimization_patterns` | inline, RVO, `[[likely]]`, `__builtin_expect` | Горячие циклы |
| `ex06_simd` | `std::simd`/`-march`, автовекторизация | Математика, обработка сигналов |

### m08_build — Сборка и распространение

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_cpp_modules` | `import`/`export`, module partitions, `import std` | Ускорение сборки, изоляция |
| `ex02_cmake_advanced` | targets, `INTERFACE`, install/export, `find_package` | Библиотеки и SDK |
| `ex03_libraries_abi` | static/shared, visibility, версионирование ABI, vcpkg/Conan | Поставка бинарников |
| `ex04_unit_tests_fuzz` | test-runner, фаззинг (`-fsanitize=fuzzer` alt.) | Качество, поиск багов |

### m09_practice — Реальные задачи и capstone

| Задание | Главный навык | Приближено к |
|---|---|---|
| `ex01_json_parser` | `variant`/`expected`/`string_view`, рекурсивный спуск | nlohmann::json |
| `ex02_binary_serialization` | `span`/`bit_cast`/endianness, версии формата | Сохранения, сетевые пакеты |
| `ex03_ecs` | SoA, `vector`, пулы, templates/concepts | Entity-Component-System |
| `ex04_thread_pool` | `thread`/`condition_variable`/`function` | Планировщик задач |
| `ex05_event_bus` | `function`, `any`, type-erasure, подписки | Шина событий движка |
| `ex06_state_machine` | `variant`/`visit`, проверяемые переходы | AI/NPC, меню |
| `ex07_arena_allocator` | `pmr`, alignment, кадровые аллокации | Игровой аллокатор |
| `ex08_raytracer` | векторы, `optional`, квадратные уравнения, ASCII-вывод | Рендер (учебный) |
| `ex09_lua_bindings` | C-API/stack, RAII-обёртки, `expected` | Мост в основной курс (th18) |
| `ex10_capstone_engine` | ECS + event bus + FSM, CMake, `ctest` | Мини-движок |

### m10_language_future — Язык завтрашнего дня (C++26+)

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_contracts` | контракты: `pre`/`post`/`contract_assert`, `__cpp_contracts` (нужен `-lstdc++exp`) | Инварианты API, контейнеры, ядро |
| `ex02_reflection` | фолбэк через structured bindings (`fields_count`/`to_tuple`) + заметка о `^^`/`std::meta` | Сериализация/ORM/биндинги без макросов |
| `ex03_pattern_matching` | `std::variant` + `visit`/`overloaded` вместо `inspect`/`is` | Разбор вариантов, маршрутизация |
| `ex04_senders_execution` | мини sender/receiver: `then`/`upon_error`/`when_all`/`sync_wait`, `expected` | Асинхронные пайплайны, планировщики |
| `ex05_simd_hive_containers` | `<experimental/simd>` + `std::inplace_vector` (нет `std::simd`/`<hive>`) | Численные вычисления, аллокационно-чувствительный код |
| `ex06_comptime_meta` | `consteval`, `if consteval`, compile-time таблицы/хеши, `static_assert` | Генерация таблиц, ключи без аллокаций |

### m11_networking_io — Сеть и ввод-вывод

> На Windows все задания линкуют Winsock: запускайте с `-Extra '-lws2_32'`.
> Для детерминизма тесты используют loopback и эфемерный порт (не печатается).

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_sockets` | RAII-обёртки `WsaGuard`/`Socket`, `bind`/`accept`/`connect`, loopback | Клиент-сервер, нетворкинг |
| `ex02_event_loop` | реактор на `select`, `fd_set`, мультиплексирование | asio/libuv, прокси |
| `ex03_protocol_framing` | length-prefixed фрейминг, `FrameDecoder`, partial read | Протоколы, RPC |
| `ex04_timeouts_cancellation` | `select` с `timeval`, `poll_readable`, `jthread`/`stop_token` | Надёжные сетевые сервисы |
| `ex05_http_server` | разбор HTTP/1.1, маршруты, `Content-Length`, ответ | REST API, статика |
| `ex06_async_io` | композиция операций: `read_sender`/`then`/`when_all`/`sync_wait` | Высоконагруженные серверы |

### m12_parallel_gpu — Параллелизм и GPU

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_parallel_stl` | execution policies (`seq`/`par`/`par_unseq`), `reduce`/`transform_reduce`/`for_each` | Массовая обработка данных |
| `ex02_task_scheduling` | граф задач (DAG), indegree-планировщик, futures-композиция | Планировщики, игровой тик |
| `ex03_simd_advanced` | `<experimental/simd>`, маски, `where`/`clamp`, `reduce` | DSP, кодеки, ML-препроцесс |
| `ex04_gpu_offload` | модель offload: host/device-память, запуск ядра сеткой | GPGPU, вычисления |
| `ex05_false_sharing` | `alignas(64)`, линии кэша, кэш-пинг-понг | Масштабирование сервисов |
| `ex06_parallel_profiling` | измерение speedup, закон Амдала, память-bound | Оптимизация под нагрузкой |

> Тема SIMD перекликается с `m07_performance/ex06_simd`; `m12` идёт дальше —
> параллельные политики, планирование задач и модель GPU. На текущем тулчейне
> TBB/GPU-рантайма нет: `par`-алгоритмы работают на **последовательном
> бэкенде**, а `ex04` демонстрирует **модель** offload. `ex05`/`ex06` печатают
> время (недетерминировано).

### m13_interop_ffi — Совместимость и FFI

> Реальные: `extern "C"`/opaque-handles (`ex01`), `LoadLibraryA`+`GetProcAddress`
> (`ex02`), ABI-плагины (`ex03`), встраивание CPython (`ex05`, см. флаги ниже).
> `ex04` (WASM) и `ex06` (Rust) — **модели**: `emcc`/`rustc` в среде нет.

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_c_abi` | `extern "C"`, opaque-хендлы, указатели на функции, POD-границы | Библиотеки, SDK |
| `ex02_dynamic_loading` | RAII над `LoadLibraryA`/`GetProcAddress`, ошибки модуля/символа | Плагины, драйверы |
| `ex03_plugin_system` | C-ABI `PluginApi`, проверка версии, жизненный цикл | Хосты плагинов |
| `ex04_wasm_embind` | модель WASM: линейная память, таблица экспортов (embind-стиль) | Браузерные приложения |
| `ex05_python_embedding` | CPython C-API, `PyConfig`, собственный модуль, ссылки | Скриптинг, ML-пайплайны |
| `ex06_rust_ffi` | `#[repr(C)]`-конвенции, `Option<Box<T>>`→`nullptr`, владение | Микросервисы, компоненты |

> `ex05` запускается с
> `-Extra '-IC:/msys64/ucrt64/include/python3.12 -LC:/msys64/ucrt64/lib -lpython3.12'`
> (msys2 ucrt64 Python 3.12); без заголовков работает симуляция с тем же
> результатом.

### m14_quality — Качество и инженерные практики

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_property_based` | свойства-инварианты, генераторы, shrinking, фиксированный seed | Парсеры, кодеки, форматы |
| `ex02_fuzzing` | мутационный фаззинг, корпус, покрытие ветвей как обратная связь | Безопасность, устойчивость |
| `ex03_coverage` | function/branch coverage; `gcov --coverage` (проверен, `gcov 16.2.0`) | CI-качество |
| `ex04_static_analysis` | `static_assert`/`[[nodiscard]]`/`consteval`, `if consteval`, `g++ -fanalyzer` | Линтинг, ревью |
| `ex05_benchmarking_ci` | медиана, сток против оптимизации, регрессионный гейт | Производительность в CI |
| `ex06_ci_matrix` | матрица ОС×компилятор×стандарт, pass/fail/skip, YAML | DevOps, релизы |

> `ex05` печатает время (бенчмарк, недетерминировано). `ex02`/`ex03`/`ex04`
> используют детерминированные модели/фикстуры; `libFuzzer`/`clang-tidy` в среде
> нет, но `gcov` и `g++ -fanalyzer` **доступны и проверены**.

> Углубляет `m08_build/ex04_unit_tests_fuzz`: здесь — генеративные тесты,
> фаззинг «на масштабе» и качество в CI.

### m15_systems — Системное программирование

| Задание | Синтаксис / тема | Реальная задача / где применять |
|---|---|---|
| `ex01_mmap_filesystem` | `std::filesystem`, `mmap`/`MapViewOfFile`, большие файлы | БД, логгеры, кэши |
| `ex02_processes_ipc` | процессы, pipes, shared memory, signals | Утилиты, координация сервисов |
| `ex03_real_time` | latency, приоритеты, lock-free очереди, детерминизм | Аудио, HFT, игровой цикл |
| `ex04_freestanding` | `-ffreestanding`, без исключений/RTTI, регистры | Embedded, ОС, микроконтроллеры |
| `ex05_memory_debugging` | кастомные аллокаторы, guard pages, детекторы | Отладка утечек и UB |
| `ex06_hardening_security` | переполнения, hardening-флаги, безопасные границы | Приложения с угрозами |

> Windows-API (`CreateFileMapping`/`MapViewOfFile`, `CreateProcessW`/`CreatePipe`);
> POSIX-аналоги (`mmap`, `fork`/`exec`) описаны в `.md`. `ex02` запускает
> дочерний процесс (сам exe с `--child`). ASan/UBSan в MinGW нет — `ex05`
> использует канарейки и арену.

### m16_projects — Прикладные проекты (capstone-серия)

| Проект | Тема | Что демонстрирует |
|---|---|---|
| `ex01_kv_store` | in-memory хранилище + персистентность | структуры данных, I/O, тесты |
| `ex02_toy_compiler` | lexer → parser → bytecode VM | рефлексия/parsing, интерпретатор |
| `ex03_http_service` | сетевой сервис на потоках/пуле | сокеты, конкурентность, протокол |
| `ex04_profiler` | сэмплирующий профайлер | сигналы/потоки, символы, сбор |
| `ex05_plugin_host` | хост плагинов со стабильным C-ABI | ABI, загрузка, версии |
| `ex06_render_pipeline` | ECS + рендер-пайплайн | ECS, параллелизм, графика |

> Проекты `m16` собирают воедино модули `m10`–`m15` и заменяют единственный
> capstone `m09` полноценной серией.

---

## 7. Трекер прогресса

Статусы: `[ ]` — не начато, `[~]` — в работе, `[x]` — готово. Отмечайте здесь,
чтобы видеть, **что изучили**, а **что предстоит**.

### m00_tooling

- [x] `ex01_workflow`
- [x] `ex02_compiler_flags`
- [x] `ex03_debug_sanitizers`
- [x] `ex04_cmake_multi`
- [x] `ex05_testing_doctest`

### m01_syntax — обзор всего синтаксиса

- [x] `ex01_types_literals`
- [x] `ex02_decl_init_const`
- [x] `ex03_operators`
- [x] `ex04_control_flow`
- [x] `ex05_functions`
- [x] `ex06_scope_namespaces`
- [x] `ex07_pointers_refs`
- [x] `ex08_arrays_strings`
- [x] `ex09_structs_enums_unions`
- [x] `ex10_classes`
- [x] `ex11_inheritance_polymorphism`
- [x] `ex12_lambdas`
- [x] `ex13_templates_intro`
- [x] `ex14_exceptions_basics`
- [x] `ex15_cpp20_syntax`
- [x] `ex16_cpp23_syntax`
- [x] `ex17_cpp26_preview`

### m02_ownership

- [x] `ex01_raii`
- [x] `ex02_special_members`
- [x] `ex03_smart_pointers`
- [x] `ex04_move_semantics`
- [x] `ex05_perfect_forwarding`
- [x] `ex06_copy_elision`

### m03_templates

- [x] `ex01_template_basics`
- [x] `ex02_specialization_sfinae`
- [x] `ex03_type_traits`
- [x] `ex04_concepts`
- [x] `ex05_variadic_folds`
- [x] `ex06_compile_time`
- [x] `ex07_crtp_type_erasure`

### m04_stl

- [x] `ex01_sequence_containers`
- [x] `ex02_associative_containers`
- [x] `ex03_adaptors_heaps`
- [x] `ex04_iterators`
- [x] `ex05_algorithms_ranges`
- [x] `ex06_views_composition`
- [x] `ex07_allocators_pmr`
- [x] `ex08_strings_charconv`
- [x] `ex09_chrono`
- [x] `ex10_functional`
- [x] `ex11_sum_product_types`

### m05_errors

- [x] `ex01_exceptions_safety`
- [x] `ex02_noexcept`
- [x] `ex03_expected_monadic`
- [x] `ex04_error_strategies`

### m06_concurrency

- [x] `ex01_threads_jthread`
- [x] `ex02_mutex_locks_cv`
- [x] `ex03_atomics_memory_order`
- [x] `ex04_async_futures`
- [x] `ex05_coroutines`
- [x] `ex06_execution_senders`

### m07_performance

- [x] `ex01_object_model_alignment`
- [x] `ex02_cache_data_oriented`
- [x] `ex03_benchmarking`
- [x] `ex04_profiling`
- [x] `ex05_optimization_patterns`
- [x] `ex06_simd`

### m08_build

- [x] `ex01_cpp_modules`
- [x] `ex02_cmake_advanced`
- [x] `ex03_libraries_abi`
- [x] `ex04_unit_tests_fuzz`

### m09_practice

- [x] `ex01_json_parser`
- [x] `ex02_binary_serialization`
- [x] `ex03_ecs`
- [x] `ex04_thread_pool`
- [x] `ex05_event_bus`
- [x] `ex06_state_machine`
- [x] `ex07_arena_allocator`
- [x] `ex08_raytracer`
- [x] `ex09_lua_bindings`
- [x] `ex10_capstone_engine`

### m10_language_future

- [x] `ex01_contracts`
- [x] `ex02_reflection`
- [x] `ex03_pattern_matching`
- [x] `ex04_senders_execution`
- [x] `ex05_simd_hive_containers`
- [x] `ex06_comptime_meta`

### m11_networking_io

- [x] `ex01_sockets`
- [x] `ex02_event_loop`
- [x] `ex03_protocol_framing`
- [x] `ex04_timeouts_cancellation`
- [x] `ex05_http_server`
- [x] `ex06_async_io`

### m12_parallel_gpu

- [x] `ex01_parallel_stl`
- [x] `ex02_task_scheduling`
- [x] `ex03_simd_advanced`
- [x] `ex04_gpu_offload`
- [x] `ex05_false_sharing`
- [x] `ex06_parallel_profiling`

### m13_interop_ffi

- [x] `ex01_c_abi`
- [x] `ex02_dynamic_loading`
- [x] `ex03_plugin_system`
- [x] `ex04_wasm_embind`
- [x] `ex05_python_embedding`
- [x] `ex06_rust_ffi`

### m14_quality

- [x] `ex01_property_based`
- [x] `ex02_fuzzing`
- [x] `ex03_coverage`
- [x] `ex04_static_analysis`
- [x] `ex05_benchmarking_ci`
- [x] `ex06_ci_matrix`

### m15_systems

- [x] `ex01_mmap_filesystem`
- [x] `ex02_processes_ipc`
- [x] `ex03_real_time`
- [x] `ex04_freestanding`
- [x] `ex05_memory_debugging`
- [x] `ex06_hardening_security`

### m16_projects

- [ ] `ex01_kv_store`
- [ ] `ex02_toy_compiler`
- [ ] `ex03_http_service`
- [ ] `ex04_profiler`
- [ ] `ex05_plugin_host`
- [ ] `ex06_render_pipeline`

## 8. Рекомендуемые «волны» прохождения

1. **Волна 1 (фундамент):** `m00` → `m01` целиком. Даёт полное повторение
   синтаксиса и рабочий процесс.
2. **Волна 2 (владение и обобщения):** `m02` → `m03` → `m04`.
3. **Волна 3 (надёжность):** `m05` → `m06`.
4. **Волна 4 (мастерство):** `m07` → `m08`.
5. **Волна 5 (практика):** `m09`, включая capstone.
6. **Волна 6 (профессиональный уровень):** `m10` → `m11` → `m12` → `m13`.
7. **Волна 7 (зрелость):** `m14` → `m15` → прикладные проекты `m16`.

После каждой волны обновляйте трекер в разделе 7 и запись в `../CHANGELOG.md`.

## 9. Критерии готовности задания

- [ ] `.cpp` компилируется `run.ps1` без предупреждений.
- [ ] `.cpp` содержит `main` и демонстрирует ровно заявленную тему.
- [ ] `.md` содержит теорию, синтаксис, задачу и раздел «где применяется».
- [ ] Вывод детерминирован (или явно описан недетерминизм).
- [ ] Для задач с ресурсами/потоками/UB — прогон под санитайзером.

## 10. Источники

- ISO C++: `https://isocpp.org/` · cppreference: `https://en.cppreference.com/`
- GCC 16 C++ status: `https://gcc.gnu.org/projects/cxx-status.html`
- C++ Core Guidelines: `https://isocpp.github.io/CppCoreGuidelines/`
- Словарь терминов курса — [`../GLOSSARY.md`](../GLOSSARY.md).
