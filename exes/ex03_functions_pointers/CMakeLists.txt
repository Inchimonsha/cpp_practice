# ex03 — сборка (без SFML, чистый C++20).

# Логика отряда, тестируемая без интерфейса.
add_library(ex03_logic STATIC
    shared/src/squad.cpp
)
target_include_directories(ex03_logic PUBLIC shared/include)
target_link_libraries(ex03_logic PUBLIC cpp_course_common)
cpp_course_apply_warnings(ex03_logic PRIVATE)

# --- Решение -----------------------------------------------------------------
add_executable(ex03_solution solution/main.cpp)
target_link_libraries(ex03_solution PRIVATE ex03_logic)
cpp_course_apply_warnings(ex03_solution PRIVATE)

# --- Каркас (starter) ----------------------------------------------------------
add_executable(ex03_starter starter/main.cpp)
target_link_libraries(ex03_starter PRIVATE ex03_logic)
cpp_course_apply_warnings(ex03_starter PRIVATE)

# --- Тесты --------------------------------------------------------------------
if(CPP_COURSE_BUILD_TESTS)
    add_executable(ex03_tests
        tests/main.cpp
        tests/test_squad.cpp
    )
    target_link_libraries(ex03_tests PRIVATE ex03_logic)
    cpp_course_apply_warnings(ex03_tests PRIVATE)
    add_test(NAME ex03_tests COMMAND ex03_tests)
endif()