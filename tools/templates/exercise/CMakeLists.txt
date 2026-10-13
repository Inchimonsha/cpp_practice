# Сборка упражнения.
# Вставляется в exes/exNN_name/CMakeLists.txt.

file(GLOB_RECURSE exercise_sources CONFIGURE_DEPENDS "solution/*.cpp")
file(GLOB_RECURSE exercise_starter_sources CONFIGURE_DEPENDS "starter/*.cpp")
file(GLOB_RECURSE exercise_test_sources CONFIGURE_DEPENDS "tests/*.cpp")

# Решение (исполняемый файл)
add_executable(exNN_solution ${exercise_sources})
target_link_libraries(exNN_solution PRIVATE cpp_course_common)
cpp_course_apply_warnings(exNN_solution PRIVATE)

if(TARGET cpp_course::sfml)
    cpp_course_link_sfml(exNN_solution)
endif()

# Тесты (используют решение как библиотеку или собирают свои TU)
add_executable(exNN_tests ${exercise_test_sources})
target_link_libraries(exNN_tests PRIVATE cpp_course_common)
cpp_course_apply_warnings(exNN_tests PRIVATE)

if(CPP_COURSE_BUILD_TESTS)
    add_test(NAME exNN_tests COMMAND exNN_tests)
endif()