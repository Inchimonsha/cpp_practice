# cmake/compiler_warnings.cmake
# Единая политика предупреждений для всех целей курса.
#
# Функция cpp_course_apply_warnings(target [INTERFACE|PUBLIC|PRIVATE]):
#   применяет набор предупреждений к target.

function(cpp_course_apply_warnings target scope)
    if(NOT scope)
        set(scope PRIVATE)
    endif()

    if(MSVC)
        target_compile_options(${target} ${scope} /W4 /permissive-)
    else()
        # GCC 13 proven; медианный уровень для обучения.
        target_compile_options(${target} ${scope}
            -Wall -Wextra -Wpedantic
            -Wshadow
            -Wconversion -Wsign-conversion
            -Wnull-dereference
            -Wformat=2
            -Wundef
            -Wdeprecated
            -Wnon-virtual-dtor
        )
    endif()

    if(CPP_COURSE_WARNINGS_AS_ERRORS)
        if(MSVC)
            target_compile_options(${target} ${scope} /WX)
        else()
            target_compile_options(${target} ${scope} -Werror)
        endif()
    endif()
endfunction()