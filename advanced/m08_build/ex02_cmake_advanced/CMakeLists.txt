cmake_minimum_required(VERSION 3.20)
project(cmake_advanced LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cc_warnings INTERFACE)
target_compile_options(cc_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow)

add_library(shapes STATIC src/shape.cpp)
target_include_directories(shapes PUBLIC include)
target_link_libraries(shapes PRIVATE cc_warnings)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE shapes cc_warnings)

include(GNUInstallDirs)
install(TARGETS shapes app
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
