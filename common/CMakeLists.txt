add_library(cpp_course_common STATIC
    src/log.cpp
)

add_library(cpp_course::common ALIAS cpp_course_common)

target_include_directories(cpp_course_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# Флаги и стандарт — через общий таргет курса.
target_link_libraries(cpp_course_common PUBLIC cpp_course_flags)

if(CPP_COURSE_BUILD_EXAMPLES)
    add_executable(cpp_course_demo examples/demo.cpp)
    target_link_libraries(cpp_course_demo PRIVATE cpp_course_common)
    cpp_course_apply_warnings(cpp_course_demo PRIVATE)
endif()

if(CPP_COURSE_BUILD_TESTS)
    add_executable(common_tests
        tests/main.cpp
        tests/test_vec2.cpp
        tests/test_log.cpp
    )
    target_link_libraries(common_tests PRIVATE cpp_course_common)
    cpp_course_apply_warnings(common_tests PRIVATE)

    add_test(NAME common_tests COMMAND common_tests)
endif()