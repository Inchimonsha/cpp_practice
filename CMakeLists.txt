cmake_minimum_required(VERSION 3.20)

project(cpp_course
    VERSION 0.1.0
    DESCRIPTION "Учебный курс: C++ и Lua, разработка 2D-игр и песочниц (SFML)"
    LANGUAGES CXX C
)

# Хелперы курса в cmake/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# --- Опции курса -----------------------------------------------------------
include(options)
include(compiler_warnings)

# Общий интерфейсный таргет: флаги и стандарт для всех целей курса.
add_library(cpp_course_flags INTERFACE)
target_compile_features(cpp_course_flags INTERFACE cxx_std_20)
cpp_course_apply_warnings(cpp_course_flags INTERFACE)
set_target_properties(cpp_course_flags PROPERTIES
    INTERFACE_POSITION_INDEPENDENT_CODE ON
)

# --- Внешние зависимости (SFML, Lua) ---------------------------------------
include(sfml)
include(lua)

enable_testing()

# --- Модули курса -----------------------------------------------------------
add_subdirectory(common)

if(CPP_COURSE_BUILD_EXAMPLES)
    add_subdirectory(theory)
    add_subdirectory(exes)
endif()

add_subdirectory(prjs)

# --- Сводка конфигурации ----------------------------------------------------
message(STATUS "[cpp_course] C++ standard : C++20")
message(STATUS "[cpp_course] SFML 3       : ${CPP_COURSE_SFML_FOUND}")
if(CPP_COURSE_SFML_FOUND)
    message(STATUS "[cpp_course]   (SFML root: ${CPP_COURSE_SFML_ROOT})")
endif()
message(STATUS "[cpp_course] Lua 5.4      : ${CPP_COURSE_LUA_FOUND}")
message(STATUS "[cpp_course] Tests        : ${CPP_COURSE_BUILD_TESTS}")
message(STATUS "[cpp_course] Examples     : ${CPP_COURSE_BUILD_EXAMPLES}")