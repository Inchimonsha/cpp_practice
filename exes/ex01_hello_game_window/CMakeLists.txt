# ex01 — сборка.

# Логика, тестируемая без окна.
add_library(ex01_logic STATIC
    shared/src/fps_counter.cpp
)
target_include_directories(ex01_logic PUBLIC shared/include)
target_link_libraries(ex01_logic PUBLIC cpp_course_common)
cpp_course_apply_warnings(ex01_logic PRIVATE)

# --- Решение ----------------------------------------------------------------
add_executable(ex01_solution solution/main.cpp)
target_link_libraries(ex01_solution PRIVATE ex01_logic)
cpp_course_apply_warnings(ex01_solution PRIVATE)

if(TARGET cpp_course::sfml)
    cpp_course_link_sfml(ex01_solution)
else()
    message(STATUS "ex01: SFML не найден — окно не построится; выполните: python tools/fetch_deps.py")
endif()

# --- Каркас (starter) ---------------------------------------------------------
add_executable(ex01_starter starter/main.cpp)
target_link_libraries(ex01_starter PRIVATE ex01_logic)
cpp_course_apply_warnings(ex01_starter PRIVATE)

if(TARGET cpp_course::sfml)
    cpp_course_link_sfml(ex01_starter)
endif()

# --- Тесты -------------------------------------------------------------------
if(CPP_COURSE_BUILD_TESTS)
    add_executable(ex01_tests
        tests/main.cpp
        tests/test_fps_counter.cpp
    )
    target_link_libraries(ex01_tests PRIVATE ex01_logic)
    cpp_course_apply_warnings(ex01_tests PRIVATE)
    add_test(NAME ex01_tests COMMAND ex01_tests)

    # Опциональный smoke-тест окна: только по явному флагу.
    option(CPP_COURSE_RUN_WINDOW_TESTS
           "Запускать тесты, открывающие окно (требует интерактивной сессии)" OFF)
    if(CPP_COURSE_RUN_WINDOW_TESTS AND TARGET cpp_course::sfml)
        add_executable(ex01_window_smoke tests/test_window_smoke.cpp)
        target_link_libraries(ex01_window_smoke PRIVATE ex01_logic)
        cpp_course_apply_warnings(ex01_window_smoke PRIVATE)
        cpp_course_link_sfml(ex01_window_smoke)
        add_test(NAME ex01_window_smoke COMMAND ex01_window_smoke)
    else()
        message(STATUS "ex01: smoke-тест окна пропущен (CPP_COURSE_RUN_WINDOW_TESTS=OFF или нет SFML)")
    endif()
endif()