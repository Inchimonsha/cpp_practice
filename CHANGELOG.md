# CHANGELOG

Все заметные изменения репозитория курса. Формат — [Keep a Changelog](https://keepachangelog.com/ru/1.1.0/),
версионирование — [SemVer](https://semver.org/lang/ru/).

## [Unreleased]

### Добавлено

- Раздел **`advanced/`** — продвинутый курс «Advanced C++ 2026» (C++23 с
  обзором C++26): синтаксис языка и практика на реальных задачах.
  - `advanced/README.md` — план курса: модули `m00_tooling` … `m09_practice`,
    ~75 заданий с краткой теорией и синтаксисом, трекер прогресса.
  - Формат курса — *single-file*: каждое задание это один `.cpp` плюс
    `exNN_title.md` с теорией, синтаксисом и задачей.
  - Скрипты запуска одного файла: `advanced/run.ps1`, `run.cmd`, `run.sh`
    (поиск UCRT-`g++`, флаги предупреждений, `-lstdc++exp` для `std::print`,
    санитайзеры или `_GLIBCXX_ASSERTIONS` на MinGW).
  - Первое задание `m00_tooling/ex01_workflow` (`.md` + `.cpp`).
- Модуль **`m00_tooling`** (завершён): `ex02_compiler_flags`,
  `ex03_debug_sanitizers`, `ex04_cmake_multi` (многофайловый CMake-проект),
  `ex05_testing_doctest` (минимальный test-runner).
- Модуль **`m01_syntax`** (завершён, 17 заданий): `ex01_types_literals` …
  `ex17_cpp26_preview` — типы, объявления, операторы, поток управления,
  функции, области видимости, указатели, массивы/строки, структуры/enum/union,
  классы, наследование, лямбды, шаблоны, исключения, синтаксис C++20/23/26.
- Модуль **`m02_ownership`** (завершён, 6 заданий): `ex01_raii`,
  `ex02_special_members`, `ex03_smart_pointers`, `ex04_move_semantics`,
  `ex05_perfect_forwarding`, `ex06_copy_elision`.
- Модуль **`m03_templates`** (завершён, 7 заданий): `ex01_template_basics`,
  `ex02_specialization_sfinae`, `ex03_type_traits`, `ex04_concepts`,
  `ex05_variadic_folds`, `ex06_compile_time`, `ex07_crtp_type_erasure`.
- Модуль **`m04_stl`** (завершён, 11 заданий): `ex01_sequence_containers`,
  `ex02_associative_containers`, `ex03_adaptors_heaps`, `ex04_iterators`,
  `ex05_algorithms_ranges`, `ex06_views_composition`, `ex07_allocators_pmr`,
  `ex08_strings_charconv`, `ex09_chrono`, `ex10_functional`,
  `ex11_sum_product_types`.
- Модуль **`m05_errors`** (завершён, 4 задания): `ex01_exceptions_safety`,
  `ex02_noexcept`, `ex03_expected_monadic`, `ex04_error_strategies`.
- Модуль **`m06_concurrency`** (завершён, 6 заданий): `ex01_threads_jthread`,
  `ex02_mutex_locks_cv`, `ex03_atomics_memory_order`, `ex04_async_futures`,
  `ex05_coroutines`, `ex06_execution_senders`.
- Модуль **`m07_performance`** (завершён, 6 заданий):
  `ex01_object_model_alignment`, `ex02_cache_data_oriented`, `ex03_benchmarking`,
  `ex04_profiling`, `ex05_optimization_patterns`, `ex06_simd`.
- Модуль **`m08_build`** (завершён, 4 задания): `ex01_cpp_modules`
  (многомодульный пример `math.cppm`+`main.cpp`+`build.cmd`),
  `ex02_cmake_advanced` (CMake-проект с `INTERFACE`/`STATIC` целями и install),
  `ex03_libraries_abi`, `ex04_unit_tests_fuzz`.
- Модуль **`m09_practice`** (завершён, 10 заданий): `ex01_json_parser`,
  `ex02_binary_serialization`, `ex03_ecs`, `ex04_thread_pool`, `ex05_event_bus`,
  `ex06_state_machine`, `ex07_arena_allocator`, `ex08_raytracer`,
  `ex09_lua_bindings` (самодостаточная модель Lua C API на `variant`/`span`/
  `expected`), `ex10_capstone_engine` (многофайловый CMake-проект: ECS + event bus
  + FSM, сборка `engine`/`engine_demo`/`engine_tests`, `ctest`, `build.cmd`).
- Модуль **`m10_language_future`** (завершён, 6 заданий): `ex01_contracts`
  (C++26-контракты `pre`/`post`/`contract_assert`, `-lstdc++exp`),
  `ex02_reflection` (автообход полей агрегатов через structured bindings —
  фолбэк при отсутствии `^^`/`std::meta`), `ex03_pattern_matching`
  (`std::variant` + `visit`/`overloaded` вместо `inspect`/`is`),
  `ex04_senders_execution` (расширенная мини-модель sender/receiver:
  `then`/`upon_error`/`when_all`/`sync_wait`, канал ошибок, `std::expected`),
  `ex05_simd_hive_containers` (`<experimental/simd>` + `std::inplace_vector` при
  отсутствии `std::simd`/`<hive>`), `ex06_comptime_meta` (`consteval`,
  `if consteval`, compile-time таблицы/хеши, `static_assert`).
- **План `advanced/`: профессиональный уровень.** Написаны `m10`–`m15`;
  остаются `m16_projects` (capstone-серия: `kv_store`, `toy_compiler`,
  `http_service`, `profiler`, `plugin_host`, `render_pipeline`).
- Модуль **`m11_networking_io`** (завершён, 6 заданий): `ex01_sockets`
  (RAII-обёртки `WsaGuard`/`Socket`, TCP loopback, эфемерный порт),
  `ex02_event_loop` (реактор на `select`), `ex03_protocol_framing`
  (length-prefixed `FrameDecoder`, partial read), `ex04_timeouts_cancellation`
  (`poll_readable` на `select`, `std::jthread`/`stop_token`), `ex05_http_server`
  (разбор HTTP/1.1, маршруты, `Content-Length`), `ex06_async_io` (композиция
  операций `read_sender`/`then`/`when_all`/`sync_wait` над сокетами). На Windows
  требуется `-Extra '-lws2_32'`.
- Модуль **`m12_parallel_gpu`** (завершён, 6 заданий): `ex01_parallel_stl`
  (execution policies: `seq`/`par`/`par_unseq`, `reduce`/`transform_reduce`/`for_each`;
  бэкенд `libstdc++` — последовательный при отсутствии TBB),
  `ex02_task_scheduling` (планировщик по графу зависимостей: indegree + пул
  `std::jthread` + композиция `std::future`), `ex03_simd_advanced`
  (`<experimental/simd>`, маски и `where`-выражения, `clamp`/`abs`/`reduce` —
  публичного `stdx::select` в libstdc++ нет), `ex04_gpu_offload` (модель offload
  host/device + запуск ядра сеткой; реального GPU-рантайма в среде нет),
  `ex05_false_sharing` (`alignas(64)` против соседних `std::atomic`),
  `ex06_parallel_profiling` (speedup против serial, закон Амдала). `ex05`/`ex06`
  печатают время (бенчмарк, недетерминировано).
- Модуль **`m13_interop_ffi`** (завершён, 6 заданий): `ex01_c_abi`
  (`extern "C"`, opaque-handle `Counter`, вызов через указатель на функцию,
  `static_assert` на standard-layout/trivially-copyable), `ex02_dynamic_loading`
  (RAII-обёртка `SharedLibrary` над `LoadLibraryA`/`GetProcAddress`/`FreeLibrary`;
  реальный вызов `ucrtbase.dll:strlen`, обработка отсутствующего символа и
  модуля, ошибка `126`), `ex03_plugin_system` (C-ABI `PluginApi` с версией,
  фабрики `create_*`, проверка ABI и жизненный цикл `destroy`),
  `ex04_wasm_embind` (модель WASM: линейная память + таблица экспортов;
  `emcc` в среде нет), `ex05_python_embedding` (реальное встраивание CPython
  3.12 из msys2 ucrt64: `PyConfig`, `PyRun_String`, собственный модуль
  `cpp_bridge`; фолбэк-симуляция при отсутствии `Python.h`), `ex06_rust_ffi`
  (конвенции `#[repr(C)]`, `Option<Box<T>>`→`nullptr`, владение; `rustc` нет).
- Модуль **`m14_quality`** (завершён, 6 заданий): `ex01_property_based`
  (свойства-инварианты на фиксированном seed + shrinking до минимального
  контрпримера `100`), `ex02_fuzzing` (мутационный фаззинг с корпусом и
  покрытием ветвей как обратной связью), `ex03_coverage` (ручное
  инструментирование function/branch coverage; реальные команды `gcov
  --coverage` проверены на `gcov 16.2.0`), `ex04_static_analysis`
  (`static_assert`/`[[nodiscard]]`/`consteval`/`if consteval`; `g++ -fanalyzer`
  проверен — ловит double-free), `ex05_benchmarking_ci` (медиана, сток против
  оптимизации, регрессионный гейт), `ex06_ci_matrix` (матрица ОС×компилятор×
  стандарт, pass/fail/skip, YAML-фрагмент). В среде нет `libFuzzer`/`clang-tidy`,
  но есть `gcov` и `g++ -fanalyzer`.
- Модуль **`m15_systems`** (завершён, 6 заданий): `ex01_mmap_filesystem`
  (файл, отображённый в память: `CreateFileMappingW`/`MapViewOfFile`, изменение
  видно на диске), `ex02_processes_ipc` (`CreateProcessW` + `CreatePipe` +
  чтение вывода дочернего процесса, `std::signal`/`raise`), `ex03_real_time`
  (lock-free SPSC-кольцо на `std::atomic` с `acquire`/`release`, `alignas(64)`
  против ложного разделения; детерминированная проверка), `ex04_freestanding`
  (MMIO-регистры, таблица векторов прерываний; реальная сборка
  `-ffreestanding -fno-exceptions -fno-rtti` проверена — `__STDC_HOSTED__ == 0`),
  `ex05_memory_debugging` (arena-аллокатор с выравниванием + канарейки; ASan в
  MinGW нет), `ex06_hardening_security` (`std::span`-границы,
  `__builtin_add_overflow`, `volatile`-затирание секретов). Windows-API с
  POSIX-аналогами в `.md`.
- `.gitignore`: `advanced/.build/`, `gcm.cache/` — артефакты run-скриптов и кэш
  модулей GCC.

### Проверено

- Все задания `m00_tooling`, `m01_syntax` (ex01–ex17), `m02_ownership`
  (ex01–ex06), `m03_templates` (ex01–ex07), `m04_stl` (ex01–ex11),
  `m05_errors` (ex01–ex04), `m06_concurrency` (ex01–ex06), `m07_performance`
  (ex01–ex06), `m08_build` (ex01–ex04), `m09_practice` (ex01–ex10) и
  `m10_language_future` (ex01–ex06, сборка с `-Std c++26`),
  `m11_networking_io` (ex01–ex06, `-Extra '-lws2_32'`) и
  `m12_parallel_gpu` (ex01–ex06) и
  `m13_interop_ffi` (ex01–ex06; `ex05` — с
  `-IC:/msys64/ucrt64/include/python3.12 -LC:/msys64/ucrt64/lib -lpython3.12`,
  иначе фолбэк-симуляция) собираются и
  запускаются на MinGW-w64 UCRT GCC 16.2 (MSYS2 `ucrt64`) без предупреждений;
  вывод детерминирован, кроме явно отмеченных случаев (`m07_performance`,
  `m12_parallel_gpu/ex05_false_sharing`, `ex06_parallel_profiling` —
  бенчмарки, печатают время). `m08_build/ex01_cpp_modules` собран с `-fmodules-ts`,
  `m08_build/ex02_cmake_advanced` и `m09_practice/ex10_capstone_engine` — через
  CMake (MinGW Makefiles); в capstone `ctest` проходит 1/1. Проверены фолбэки
  `m10` на GCC 16.2: нет `^^`/`std::meta`, `inspect`/`is`, `std::simd`, `<hive>`,
  `__cpp_lib_senders` — задания используют альтернативы (<experimental/simd>,
  `std::inplace_vector`, `std::visit`, structured bindings).


## [0.2.0] — 2026-09-19

### Добавлено

- Блок A этапа 1 (Основы C++). Новые теоретические темы — полные разделы
  (README, theory, cheatsheet, pitfalls, sources, компилируемые примеры):
  - **th02_cpp_basics_syntax** — типы и их диапазоны, `const`/`constexpr`/
    `static`, операторы, условия, `switch`, циклы;
  - **th03_functions_scope** — функции, перегрузка, параметры, области
    видимости, `static`-переменные, разделение `.hpp`/`.cpp`;
  - **th04_pointers_references** — ссылки, указатели, стек/куча, время жизни;
  - **th05_arrays_containers** — `std::array`, `std::vector`, итераторы,
    `std::span`;
  - **th06_strings** — `std::string`, `std::string_view`, кодировки.
- Новые задания с полным комплектом (`task.md`, `shared/`, `starter/`,
  `solution/`, `tests/`, `hints.md`, `review.md`):
  - **ex02_basics_loops** «Base Defense» — боевая симуляция (типы, условия,
    циклы);
  - **ex03_functions_pointers** «Squad Combat Simulator» — функции, ссылки,
    указатели;
  - **ex04_vectors_strings** «Event Log» — `std::vector`, `std::string`,
    `std::string_view`,
- Примеры тем th02–th06 подключены к общей сборке, задания ex02–ex04 — к
  `ctest` (цели `exNN_logic/solution/starter/tests`).

### Изменено

- Индекс курса в `README.md`: ссылки на th02–th06 и ex02–ex04; маппинг
  «тема → задание» зафиксирован в `CHECKLIST.md` (ex03 ⇄ th03+th04,
  ex04 ⇄ th05+th06).
- `theory/CMakeLists.txt` и `exes/CMakeLists.txt`: подключены новые каталоги.

### Исправлено

- Ошибки компиляции примеров (C++20): `u8`-литерал в `std::string`,
  `-Wconversion` в `average(std::span)`, неиспользуемые переменные в
  стартерах.

### Проверено

- Чистая пересборка без предупреждений на MinGW-w64 UCRT GCC 16.2
  (MSYS2 `ucrt64`).
- `ctest` — 6/6 тестов (`common`, `ex01`–`ex04`, `prj01`).
- Smoke-запуск `ex02_solution`, `ex03_solution`, `ex04_solution` (победа,
  выход из боя, отряд, реплика журнала).

## [0.1.0] — 2026-09-19

### Добавлено

- Каркас курса: корневая документация (`README`, `AGENTS`, `ROADMAP`,
  `GLOSSARY`, `CHECKLIST`, `CHANGELOG`), `LICENSE` (MIT), `.gitignore`.
- `CMakeLists.txt` и `cmake/` — общая конфигурация сборки (C++20), опции
  включения SFML/Lua, хелперы предупреждений.
- Модуль `common/` — переиспользуемая C++20-библиотека:
  - `cpp_course/test.hpp` — минимальный хедер-онли тестовый фреймворк;
  - `cpp_course/math/vec2.hpp` — векторная математика;
  - `cpp_course/log.hpp` — минимальный логгер;
  - демо `cpp_course_demo` и тесты `common_tests`.
- Первая теоретическая тема **th01_cpp_project_setup** «Настройка проекта»
  (установка компилятора/CMake, IDE, Git, Debug/Release, сборка, запуск,
  диагностика, форматирование, работа с терминалом, лицензирование ассетов).
- Первое задание **ex01_hello_game_window**: постановка (26 разделов),
  `starter/`, `solution/`, `tests/`, `hints.md`, `review.md`.
- Первый проект **prj01_arcade_game**: `brief.md`, `requirements.md`,
  `milestones.md`, `architecture.md`, `docs/`, `design/`, собираемый скелет
  `src/` + `include/`, тесты, `CMakeLists.txt`, `CHANGELOG.md`.
- `tools/`:
  - `tools/fetch_deps.py` — загрузка SFML 3.1.0 (пребилт) и Lua 5.4.9 в `third_party/`;
  - `tools/validators/check_structure.py` — проверка обязательной структуры разделов;
  - `tools/validators/check_links.py` — проверка относительных ссылок в Markdown;
  - `tools/templates/` — шаблоны темы, задания и проекта.

### Изменено

- `cmake/sfml.cmake`: на POST_BUILD рядом с exe копируются только DLL пребилта
  нужной сборки (Debug/Release) **и рантайм-DLL MinGW-тулчейна**
  (`libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll`),
  взятые рядом с `CMAKE_CXX_COMPILER`. Это устраняет падение оконных
  приложений при запуске на машинах без UCRT-рантайма в PATH.
- `common/include/cpp_course/test.hpp`: вариативный `CHECK(...)` (запятые в
  макросах), имена регистраций тестов по `__LINE__`, исправлены
  sign-conversion и конец файла.
- `common/include/cpp_course/log.hpp`: добавлен `#include <string>`.

### Исправлено

- Документация окружения: зафиксирован UCRT-тулчейн для SFML-целей.
  msvcrt-тулчейн (bare GCC 13.2) собирает и линкует SFML-приложение, но оно
  падает при запуске (`0xC0000005`, кросс-CRT heap-коррупция — диагностика
  в gdb: `HEAP[...]: Invalid address specified to RtlFreeHeap`). Диагностика и
  решение — в `theory/th01_cpp_project_setup` (`theory.md`, `pitfalls.md`,
  `sources.md`).

### Проверено

- Чистая сборка целиком: `cmake -S . -B build && cmake --build build --parallel`
  на MinGW-w64 UCRT GCC 16.2 (MSYS2 `ucrt64`), без предупреждений.
- `ctest --test-dir build --output-on-failure` — 3/3 теста проходят
  (`common_tests`, `ex01_tests`, `prj01_tests`).
- Оконные приложения `ex01_solution` и `prj01_arcade` запускаются из `build/`
  без до-настройки и работают (окно открывается, процесс жив).
- Валидаторы структуры и ссылок — проходят без ошибок.

### Известные ограничения

- SFML/Lua-зависимые цели собираются только после `python tools/fetch_deps.py`
  и настройки `CMAKE_PREFIX_PATH` (см. `README.md`).
- Полные сценарии с открытием окна требуют интерактивной сессии; в CI они
  помечаются как опциональные.
- Аудио-тесты зависят от звукового устройства.

## Секции плана

См. `ROADMAP.md` (этапы 0–10) и `CHECKLIST.md` (статусы).

---

### Как вести журнал

- Каждая значимая правка курса добавляет запись верху раздела «Добавлено».
- После ревью `common/` — рубрика «common» с уточнением версии.
- Изменения правил `AGENTS.md` — в разделе «AGENTS.md» соответствующего релиза.
- Перед объявлением блока завершённым: обновить `ROADMAP.md`, `CHECKLIST.md`, `CHANGELOG.md`.