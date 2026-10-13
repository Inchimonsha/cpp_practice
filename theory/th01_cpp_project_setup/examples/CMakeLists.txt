# Сборка примеров th01 через корневой CMake.

if(CPP_COURSE_BUILD_EXAMPLES)
    add_executable(th01_example_hello hello.cpp)
    add_executable(th01_example_diagnostics diagnostics.cpp)
    add_executable(th01_example_build_info build_info.cpp)

    foreach(target_ex IN ITEMS
            th01_example_hello
            th01_example_diagnostics
            th01_example_build_info)
        target_link_libraries(${target_ex} PRIVATE cpp_course_flags)
        cpp_course_apply_warnings(${target_ex} PRIVATE)
    endforeach()
endif()