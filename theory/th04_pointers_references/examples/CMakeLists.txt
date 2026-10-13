# Примеры th04. Сборка — CMakeLists.txt этого каталога.

if(NOT TARGET cpp_course_flags)
    message(FATAL_ERROR "Запускайте сборку из корня курса (cmake -S . -B build)")
endif()

add_executable(th04_example_references references.cpp)
add_executable(th04_example_pointers pointers.cpp)
add_executable(th04_example_stack_heap stack_heap.cpp)

foreach(target_ex IN ITEMS
        th04_example_references
        th04_example_pointers
        th04_example_stack_heap)
    target_link_libraries(${target_ex} PRIVATE cpp_course_flags)
    cpp_course_apply_warnings(${target_ex} PRIVATE)
endforeach()