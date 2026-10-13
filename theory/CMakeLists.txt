# Подключение примеров теоретических тем к общей сборке.

if(CPP_COURSE_BUILD_EXAMPLES)
    # Тема th01: настройка проекта.
    add_subdirectory(th01_cpp_project_setup/examples)

    # Блок 1. Основы C++.
    add_subdirectory(th02_cpp_basics_syntax/examples)
    add_subdirectory(th03_functions_scope/examples)
    add_subdirectory(th04_pointers_references/examples)
    add_subdirectory(th05_arrays_containers/examples)
    add_subdirectory(th06_strings/examples)
endif()