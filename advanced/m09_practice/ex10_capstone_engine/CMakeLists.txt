cmake_minimum_required(VERSION 3.20)
project(capstone_engine CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(engine
    src/game.cpp)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

add_executable(engine_demo src/main.cpp)
target_link_libraries(engine_demo PRIVATE engine)
target_compile_options(engine_demo PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
target_link_libraries(engine_demo PRIVATE stdc++exp)

enable_testing()
add_executable(engine_tests tests/test_engine.cpp)
target_link_libraries(engine_tests PRIVATE engine)
target_compile_options(engine_tests PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
target_link_libraries(engine_tests PRIVATE stdc++exp)
add_test(NAME engine_tests COMMAND engine_tests)
