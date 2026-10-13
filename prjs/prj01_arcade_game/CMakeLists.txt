# prj01 — Arcade Game: сборка.

# Модель проекта — чистая логика, тестируется без SFML.
add_library(prj01_logic STATIC
    src/score.cpp
    src/entity.cpp
    src/player.cpp
)
target_include_directories(prj01_logic PUBLIC include)
target_link_libraries(prj01_logic PUBLIC cpp_course_common)
cpp_course_apply_warnings(prj01_logic PRIVATE)

# Игровое приложение (окно SFML).
add_executable(prj01_arcade
    src/main.cpp
    src/game.cpp
)
target_link_libraries(prj01_arcade PRIVATE prj01_logic)
cpp_course_apply_warnings(prj01_arcade PRIVATE)

if(TARGET cpp_course::sfml)
    cpp_course_link_sfml(prj01_arcade)
else()
    message(STATUS "prj01: SFML не найден — exe не построится; выполните: python tools/fetch_deps.py")
endif()

# Тесты логики (всегда, без окна).
if(CPP_COURSE_BUILD_TESTS)
    add_executable(prj01_tests
        tests/main.cpp
        tests/test_score.cpp
        tests/test_entity.cpp
    )
    target_link_libraries(prj01_tests PRIVATE prj01_logic)
    cpp_course_apply_warnings(prj01_tests PRIVATE)
    add_test(NAME prj01_tests COMMAND prj01_tests)
endif()