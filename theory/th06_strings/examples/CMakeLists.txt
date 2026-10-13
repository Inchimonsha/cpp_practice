# Примеры th06. Сборка — CMakeLists.txt этого каталога.

if(NOT TARGET cpp_course_flags)
    message(FATAL_ERROR "Запускайте сборку из корня курса (cmake -S . -B build)")
endif()

add_executable(th06_example_strings_basics strings_basics.cpp)
add_executable(th06_example_string_view string_view.cpp)
add_executable(th06_example_encoding_notes encoding_notes.cpp)

foreach(target_ex IN ITEMS
        th06_example_strings_basics
        th06_example_string_view
        th06_example_encoding_notes)
    target_link_libraries(${target_ex} PRIVATE cpp_course_flags)
    cpp_course_apply_warnings(${target_ex} PRIVATE)
endforeach()