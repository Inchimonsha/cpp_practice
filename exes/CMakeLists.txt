# Подключение упражнений к общей сборке.
# Каждое упражнение добавляется как каталог; наличие SFML/Lua проверяет
# собственный CMakeLists (иначе цель просто пропускается с уведомлением).

add_subdirectory(ex01_hello_game_window)
add_subdirectory(ex02_basics_loops)
add_subdirectory(ex03_functions_pointers)
add_subdirectory(ex04_vectors_strings)