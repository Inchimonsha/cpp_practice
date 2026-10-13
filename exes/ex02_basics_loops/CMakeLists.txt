# ex02 — сборка (без SFML, чистый C++20).

# Логика боя, тестируемая без интерфейса.
add_library(ex02_logic STATIC
    shared/src/battle.cpp
)
target_include_directories(ex02_logic PUBLIC shared/include)
target_link_libraries(ex02_logic PUBLIC cpp_course_common)
cpp_course_apply_warnings(ex02_logic PRIVATE)

# --- Решение -----------------------------------------------------------------
add_executable(ex02_solution solution/main.cpp)
target_link_libraries(ex02_solution PRIVATE ex02_logic)
cpp_course_apply_warnings(ex02_solution PRIVATE)

# --- Каркас (starter) ----------------------------------------------------------
add_executable(ex02_starter starter/main.cpp)
target_link_libraries(ex02_starter PRIVATE ex02_logic)
cpp_course_apply_warnings(ex02_starter PRIVATE)

# --- Тесты --------------------------------------------------------------------
if(CPP_COURSE_BUILD_TESTS)
    add_executable(ex02_tests
        tests/main.cpp
        tests/test_battle.cpp
    )
    target_link_libraries(ex02_tests PRIVATE ex02_logic)
    cpp_course_apply_warnings(ex02_tests PRIVATE)
    add_test(NAME ex02_tests COMMAND ex02_tests)
endif()