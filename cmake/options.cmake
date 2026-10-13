# cmake/options.cmake
# Опции сборки курса.
#
# Практический выбор для учебного курса:
#   * по умолчанию тесты и примеры включены;
#   * SFML включается автоматически, если найден пребилт;
#   * Lua собирается из исходников только по явному запросу
#     (CPP_COURSE_ENABLE_LUA=ON), чтобы базовый configure не зависел от сети.

option(CPP_COURSE_BUILD_TESTS    "Собирать тесты (общие + проектов)" ON)
option(CPP_COURSE_BUILD_EXAMPLES "Собирать примеры и упражнения"      ON)
option(CPP_COURSE_WARNINGS_AS_ERRORS "Трактовать предупреждения как ошибки" OFF)

option(CPP_COURSE_ENABLE_SFML "Собирать SFML-зависимые цели (авто, если найден SFML 3)" OFF)
option(CPP_COURSE_ENABLE_LUA  "Собирать Lua 5.4.9 из исходников (FetchContent, нужен интернет)" OFF)

# Путь к пребилту SFML. По умолчанию — third_party/SFML-3.1.0 в корне курса.
set(CPP_COURSE_SFML_ROOT "" CACHE PATH
    "Папка пребилта SFML (содержит include/ и lib/); по умолчанию — third_party/SFML-3.1.0")

if(NOT CPP_COURSE_SFML_ROOT)
    set(_default_sfml_root "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SFML-3.1.0")
    if(EXISTS "${_default_sfml_root}")
        set(CPP_COURSE_SFML_ROOT "${_default_sfml_root}")
    endif()
endif()