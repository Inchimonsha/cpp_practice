# cmake/lua.cmake
# Поддержка Lua 5.4.9 в курсе.
#
# Логика:
#   1. если CPP_COURSE_ENABLE_LUA=OFF и Lua в системе не найден —
#      Lua-цели просто не создаются;
#   2. если включено или Lua найден через find_package(Lua 5.4) —
#      создаётся интерфейсный таргет cpp_course::lua;
#   3. если включено, но в системе нет Lua — исходники 5.4.9 скачиваются
#      (FetchContent) и собираются библиотекой.
#
# Проверка использования в целях:
#     if(TARGET cpp_course::lua)
#
# Контрольная сумма исходников Lua 5.4.9 (lua.org/ftp):
set(CPP_COURSE_LUA_SHA256 "2335B6C582A52654F94612BF10D2F4672805D05329AA6568B1D8CD9E5C6FB8E6")
set(CPP_COURSE_LUA_VERSION "5.4.9")
set(CPP_COURSE_LUA_URL "https://www.lua.org/ftp/lua-5.4.9.tar.gz")

set(CPP_COURSE_LUA_FOUND FALSE)

if(CPP_COURSE_ENABLE_LUA)
    # 1) Сначала ищем системную Lua.
    find_package(Lua 5.4 QUIET)
    if(Lua_FOUND)
        set(CPP_COURSE_LUA_FOUND TRUE)
        add_library(cpp_course::lua INTERFACE IMPORTED)
        set_target_properties(cpp_course::lua PROPERTIES
            INTERFACE_LINK_LIBRARIES "Lua::Lua"
        )
    else()
        # 2) Иначе собираем из исходников (FetchContent).
        include(FetchContent)
        set(FETCHCONTENT_QUIET FALSE)
        FetchContent_Declare(lua_5_4_9
            URL      "${CPP_COURSE_LUA_URL}"
            URL_HASH "SHA256=${CPP_COURSE_LUA_SHA256}"
        )
        FetchContent_GetProperties(lua_5_4_9)
        if(NOT lua_5_4_9_POPULATED)
            FetchContent_Populate(lua_5_4_9)
        endif()

        # Lua не имеет собственного CMakeLists.txt: компилируем исходники.
        file(GLOB _lua_sources "${lua_5_4_9_SOURCE_DIR}/*.c")
        list(FILTER _lua_sources EXCLUDE REGEX "(/|\\\\)(lua|luac)\\.c$")

        add_library(cpp_course_lua STATIC ${_lua_sources})
        target_include_directories(cpp_course_lua PUBLIC "${lua_5_4_9_SOURCE_DIR}")
        target_compile_definitions(cpp_course_lua PUBLIC LUA_COMPAT_5_3=1)
        set_target_properties(cpp_course_lua PROPERTIES POSITION_INDEPENDENT_CODE ON)

        set(CPP_COURSE_LUA_FOUND TRUE)
        add_library(cpp_course::lua INTERFACE IMPORTED)
        set_target_properties(cpp_course::lua PROPERTIES
            INTERFACE_LINK_LIBRARIES "cpp_course_lua"
        )
    endif()
endif()

if(NOT CPP_COURSE_LUA_FOUND)
    message(STATUS "[cpp_course] Lua не включена (CPP_COURSE_ENABLE_LUA=OFF).")
endif()