# CHECKLIST — контроль выполнения курса

Статусы: `[ ]` — не начато, `[~]` — в работе, `[x]` — готово.

---

## Этап 0. Каркас

- [x] Корневая документация (README, AGENTS, ROADMAP, GLOSSARY, CHECKLIST, CHANGELOG, LICENSE)
- [x] CMake-конфигурация и хелперы
- [x] Модуль `common/`
- [x] Секции `theory/`, `exes/`, `prjs/`, `assets/`, `tools/` с README и шаблонами
- [x] `theory/th01_cpp_project_setup`
- [x] `exes/ex01_hello_game_window`
- [x] `prjs/prj01_arcade_game` (каркас)
- [x] Валидаторы структуры и ссылок
- [x] Проверка сборки и тестов

## Модуль 0 — Организация разработки

- [x] `th01_cpp_project_setup` — зафиксировано в плане

## Модуль 1 — Основы C++

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th02 cpp_basics_syntax | [x] | ex02 | [x] |
| th03 functions_scope | [x] | ex03 | [x] |
| th04 pointers_references | [x] | ex03 | [x] |
| th05 arrays_containers | [x] | ex04 | [x] |
| th06 strings | [x] | ex04 | [x] |
| th07 structs_classes | [ ] | ex05 | [ ] |
| th08 oop | [ ] | ex05 | [ ] |
| th09 raii_smart_pointers | [ ] | ex06 | [ ] |
| th10 error_handling | [ ] | ex07 | [ ] |
| th11 modern_features | [ ] | ex09, ex10 | [ ] |
| th12 stl_algorithms | [ ] | ex08 | [ ] |
| th13 concurrency | [ ] | — | [ ] |
| th14 performance_intro | [ ] | ex34 | [ ] |

## Модуль 2 — Основы Lua

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th15 lua_basics | [ ] | ex11 | [ ] |
| th16 lua_advanced | [ ] | ex12 | [ ] |
| th17 lua_errors_data | [ ] | ex13 | [ ] |
| th18 lua_embedding | [ ] | ex14 | [ ] |

## Модуль 3 — Математика

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th19 math_coordinates_vectors | [ ] | ex15 | [ ] |
| th20 math_motion_trig | [ ] | ex15 | [ ] |
| th21 math_interpolation | [ ] | ex17 | [ ] |
| th22 math_collisions | [ ] | ex16 | [ ] |

## Модуль 4 — SFML

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th23 sfml_window_loop | [ ] | ex01, ex18, ex19 | [ ] |
| th24 sfml_graphics | [ ] | ex20 | [ ] |
| th25 sfml_view_camera | [ ] | ex21 | [ ] |
| th26 sfml_audio | [ ] | ex22 | [ ] |
| th27 sfml_advanced | [ ] | — | [ ] |

## Модуль 5 — Архитектура

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th28 game_loop_arch | [ ] | ex18 | [ ] |
| th29 state_machine_scenes | [ ] | ex23 | [ ] |
| th30 ecs_components | [ ] | ex24 | [ ] |
| th31 events_patterns | [ ] | ex25 | [ ] |
| th32 resources_di | [ ] | ex26 | [ ] |
| th33 game_data | [ ] | ex27 | [ ] |
| th34 sim_visual_split | [ ] | — | [ ] |

## Модуль 6 — 2D GameDev

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th35 2d_game_design | [ ] | — | [ ] |
| th36 2d_ui | [ ] | ex23 | [ ] |
| th37 2d_levels | [ ] | ex28, ex29 | [ ] |
| th38 2d_effects | [ ] | ex30 | [ ] |
| th39 2d_physics | [ ] | — | [ ] |
| th40 2d_npc | [ ] | ex31 | [ ] |

## Модуль 7 — Песочницы

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th41 sandbox_architecture | [ ] | ex35 | [ ] |
| th42 procedural_generation | [ ] | — | [ ] |
| th43 sandbox_economy | [ ] | — | [ ] |
| th44 sandbox_time_weather | [ ] | — | [ ] |
| th45 sandbox_world_scale | [ ] | ex35 | [ ] |
| th46 sandbox_modding | [ ] | ex32 | [ ] |

## Модуль 8 — Профессиональная разработка

| Раздел | Статус | Задание | Статус |
|---|---|---|---|
| th47 professional_planning | [ ] | — | [ ] |
| th48 git_workflow | [ ] | — | [ ] |
| th49 testing | [ ] | ex33 | [ ] |
| th50 profiling_debugging | [ ] | ex34 | [ ] |
| th51 ci_release | [ ] | — | [ ] |

## Модуль 9 — Производительность

| Раздел | Статус |
|---|---|
| th52 performance_graphics | [ ] |
| th53 performance_data | [ ] |
| th54 performance_ecs_lua | [ ] |
| th55 optimization_process | [ ] |

## Модуль 10 — Финальные проекты

| Проект | Этап 0 (каркас) | Vertical slice | Полный MVP |
|---|---|---|---|
| prj01_arcade_game | [x] | [ ] | [ ] |
| prj02_top_down_adventure | [ ] | [ ] | [ ] |
| prj03_platformer | [ ] | [ ] | [ ] |
| prj04_survival_sandbox | [ ] | [ ] | [ ] |
| prj05_lua_scripted_game | [ ] | [ ] | [ ] |
| prj06_2d_game_engine | [ ] | [ ] | [ ] |
| prj07_capstone_sandbox | [ ] | [ ] | [ ] |

## Advanced C++ 2026 (папка `advanced/`)

Отдельный продвинутый курс (C++23, синтаксис + практика). Прогресс по модулям
`m00_tooling` … `m09_practice` ведётся в трекере внутри
[`advanced/README.md`](advanced/README.md#7-трекер-прогресса).

- [x] `m00_tooling` — инструменты и single-file workflow
- [x] `m01_syntax` — полный обзор синтаксиса C++
- [x] `m02_ownership` — владение и move-семантика
- [x] `m03_templates` — шаблоны и concepts
- [x] `m04_stl` — STL на глубину
- [x] `m05_errors` — обработка ошибок
- [x] `m06_concurrency` — конкурентность
- [x] `m07_performance` — производительность
- [x] `m08_build` — модули и сборка
- [x] `m09_practice` — реальные задачи и capstone

### advanced: профессиональный уровень (m10–m15 написаны; m16 спроектирован)

- [x] `m10_language_future` — язык завтрашнего дня (C++26+)
- [x] `m11_networking_io` — сеть и ввод-вывод
- [x] `m12_parallel_gpu` — параллелизм и GPU
- [x] `m13_interop_ffi` — совместимость и FFI
- [x] `m14_quality` — качество и инженерные практики
- [x] `m15_systems` — системное программирование
- [ ] `m16_projects` — прикладные проекты (capstone-серия)