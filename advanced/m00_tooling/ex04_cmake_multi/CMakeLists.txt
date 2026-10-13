cmake_minimum_required(VERSION 3.20)
project(ex04_cmake_multi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mathx STATIC src/mathx.cpp)
target_include_directories(mathx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(mathx PUBLIC cxx_std_23)
target_compile_options(mathx PRIVATE -Wall -Wextra -Wpedantic)

add_executable(ex04_cmake_multi src/main.cpp)
target_link_libraries(ex04_cmake_multi PRIVATE mathx)
target_compile_features(ex04_cmake_multi PRIVATE cxx_std_23)
target_compile_options(ex04_cmake_multi PRIVATE -Wall -Wextra -Wpedantic)
