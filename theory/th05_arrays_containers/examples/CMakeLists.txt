# Примеры th05. Сборка — CMakeLists.txt этого каталога.

if(NOT TARGET cpp_course_flags)
    message(FATAL_ERROR "Запускайте сборку из корня курса (cmake -S . -B build)")
endif()

add_executable(th05_example_array_vs_vector array_vs_vector.cpp)
add_executable(th05_example_iterators iterators.cpp)
add_executable(th05_example_span_window span_window.cpp)

foreach(target_ex IN ITEMS
        th05_example_array_vs_vector
        th05_example_iterators
        th05_example_span_window)
    target_link_libraries(${target_ex} PRIVATE cpp_course_flags)
    cpp_course_apply_warnings(${target_ex} PRIVATE)
endforeach()