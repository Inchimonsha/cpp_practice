# ex04 — сборка (без SFML, чистый C++20).

# Логика журнала, тестируемая без интерфейса.
add_library(ex04_logic STATIC
    shared/src/event_log.cpp
)
target_include_directories(ex04_logic PUBLIC shared/include)
target_link_libraries(ex04_logic PUBLIC cpp_course_common)
cpp_course_apply_warnings(ex04_logic PRIVATE)

# --- Решение -----------------------------------------------------------------
add_executable(ex04_solution solution/main.cpp)
target_link_libraries(ex04_solution PRIVATE ex04_logic)
cpp_course_apply_warnings(ex04_solution PRIVATE)

# --- Каркас (starter) ----------------------------------------------------------
add_executable(ex04_starter starter/main.cpp)
target_link_libraries(ex04_starter PRIVATE ex04_logic)
cpp_course_apply_warnings(ex04_starter PRIVATE)

# --- Тесты --------------------------------------------------------------------
if(CPP_COURSE_BUILD_TESTS)
    add_executable(ex04_tests
        tests/main.cpp
        tests/test_event_log.cpp
    )
    target_link_libraries(ex04_tests PRIVATE ex04_logic)
    cpp_course_apply_warnings(ex04_tests PRIVATE)
    add_test(NAME ex04_tests COMMAND ex04_tests)
endif()