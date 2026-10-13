# Примеры th03. Сборка — CMakeLists.txt этого каталога.

if(NOT TARGET cpp_course_flags)
    message(FATAL_ERROR "Запускайте сборку из корня курса (cmake -S . -B build)")
endif()

add_executable(th03_example_functions_overloads functions_overloads.cpp)
add_executable(th03_example_scopes_static scopes_static.cpp)

# Демонстрация разделения .hpp/.cpp:
add_library(th03_example_fraction STATIC fraction.cpp)
target_include_directories(th03_example_fraction PUBLIC .)
add_executable(th03_example_decl_vs_def decl_vs_def.cpp)
target_link_libraries(th03_example_decl_vs_def PRIVATE th03_example_fraction)

foreach(target_ex IN ITEMS
        th03_example_functions_overloads
        th03_example_scopes_static
        th03_example_decl_vs_def)
    target_link_libraries(${target_ex} PRIVATE cpp_course_flags)
    cpp_course_apply_warnings(${target_ex} PRIVATE)
endforeach()