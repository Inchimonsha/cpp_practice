# cmake/sfml.cmake
# Обнаружение SFML 3 и хелпер для подключения его к целям курса.
#
# Использование для SFML-зависимой цели:
#     cpp_course_link_sfml(my_app)
#
# Функция создаёт интерфейсный таргет cpp_course::sfml (если SFML найден),
# линкует целевой таргет с SFML и копирует DLL пребилта рядом с исполняемым
# файлом на Windows.

set(CPP_COURSE_SFML_FOUND FALSE)

# --- Поиск SFML 3 -----------------------------------------------------------
if(DEFINED ENV{SFML_ROOT})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{SFML_ROOT}")
endif()
if(CPP_COURSE_SFML_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${CPP_COURSE_SFML_ROOT}")
endif()

if(CPP_COURSE_ENABLE_SFML)
    find_package(SFML 3 REQUIRED COMPONENTS Graphics Window Audio System Network)
else()
    find_package(SFML 3 QUIET COMPONENTS Graphics Window Audio System Network)
endif()

if(SFML_FOUND)
    set(CPP_COURSE_SFML_FOUND TRUE)
    if(NOT TARGET cpp_course::sfml)
        add_library(cpp_course::sfml INTERFACE IMPORTED)
        set_target_properties(cpp_course::sfml PROPERTIES
            INTERFACE_LINK_LIBRARIES
                "SFML::Graphics;SFML::Window;SFML::Audio;SFML::System;SFML::Network"
        )
    endif()
    if(WIN32 AND CPP_COURSE_SFML_ROOT)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            file(GLOB _sfml_dlls "${CPP_COURSE_SFML_ROOT}/bin/sfml-*-d-3.dll")
        else()
            file(GLOB _sfml_dlls "${CPP_COURSE_SFML_ROOT}/bin/sfml-*-3.dll")
        endif()

        # Пребилт SFML для MinGW собран под UCRT и требует рантайм-DLL того же
        # тулчейна. Берём их рядом с компилятором, которым собирается курс
        # (иначе — кросс-CRT heap-коррупция и падение при запуске).
        get_filename_component(_toolchain_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
        file(GLOB _mingw_runtime_dlls
             "${_toolchain_bin}/libstdc++-6.dll"
             "${_toolchain_bin}/libgcc_s_seh-1.dll"
             "${_toolchain_bin}/libwinpthread-1.dll"
             "${_toolchain_bin}/libgcc_s-1.dll")
        list(APPEND _sfml_dlls ${_mingw_runtime_dlls})
    endif()
endif()

function(cpp_course_link_sfml target)
    if(NOT TARGET cpp_course::sfml)
        message(FATAL_ERROR
            "SFML 3 не найден. Скачайте пребилт:  python tools/fetch_deps.py, "
            "или укажите CPP_COURSE_SFML_ROOT.")
    endif()
    target_link_libraries(${target} PRIVATE cpp_course::sfml)

    if(_sfml_dlls)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${_sfml_dlls}
                    $<TARGET_FILE_DIR:${target}>
            COMMENT "Копирование DLL SFML к ${target}"
        )
    endif()
endfunction()