# Стратегия тестирования: Arcade Game

## Цели

- логика (счёт, жизни, коллизии, спавн) покрыта unit-тестами **без окна**;
- GUI не тестируется в CI (окно не создаётся).

## Уровни

1. **Unit (обязательно, в `ctest`):**
   - `Score`: добавление очков, жизни, `lose_life`, game over;
   - `Score::save/load_record`: запись/чтение, отсутствие файла, повреждённый
     файл;
   - `Aabb::intersects` — основные случаи пересечения и непересечения;
   - `Player::move/clamp` — границы поля.
2. **Smoke (окно, под флагом `CPP_COURSE_RUN_WINDOW_TESTS=ON`):**
   - несколько кадров цикла, закрытие окна, exit 0.
3. **Ручной плейтест (на вехе):**
   - вертикальный срез; независимость от FPS (30/60 vs 120 FPS).

## Инструмент

`cpp_course/test.hpp` (хедер-онли, без зависимостей). Тесты регистрируются через
`add_test` в `prj01/CMakeLists.txt`:

```cmake
add_executable(prj01_tests tests/test_score.cpp tests/test_entity.cpp)
target_link_libraries(prj01_tests PRIVATE prj01_logic)
add_test(NAME prj01_tests COMMAND prj01_tests)
```

## Тест фикстура

Для `save/load_record` — временный файл в каталоге теста, удаление
`std::filesystem::remove` после проверки (никаких файлов в репозитории).

## Покрытие

Цель по строкам на модель — не ниже ~80% на момент M5 (проверка вручную через
`gcov` — опционально, th50).