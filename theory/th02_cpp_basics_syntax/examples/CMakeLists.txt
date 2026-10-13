# Примеры th02. Сборка — CMakeLists.txt этого каталога.

if(NOT TARGET cpp_course_flags)
    message(FATAL_ERROR "Запускайте сборку из корня курса (cmake -S . -B build)")
endif()

add_executable(th02_example_types_and_limits types_and_limits.cpp)
add_executable(th02_example_const_qualifiers const_qualifiers.cpp)
add_executable(th02_example_loops_conditions loops_conditions.cpp)

foreach(target_ex IN ITEMS
        th02_example_types_and_limits
        th02_example_const_qualifiers
        th02_example_loops_conditions)
    target_link_libraries(${target_ex} PRIVATE cpp_course_flags)
    cpp_course_apply_warnings(${target_ex} PRIVATE)
endforeach()