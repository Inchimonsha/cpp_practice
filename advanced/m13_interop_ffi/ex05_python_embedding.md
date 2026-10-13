# m13_interop_ffi/ex05_python_embedding — встраивание CPython в C++

> Модуль m13_interop_ffi · Сложность ★★★ · C++23 · Запуск:
> `./run.ps1 m13_interop_ffi/ex05_python_embedding -Extra '-IC:/msys64/ucrt64/include/python3.12 -LC:/msys64/ucrt64/lib -lpython3.12'`
>
> Без этих флагов `Python.h` не найден, и работает **симуляция** с тем же
> результатом (код возврата тот же).

## Теория

**Встраивание** (embedding) — использование интерпретатора Python *внутри*
C++-приложения. Это позволяет дать пользователю скриптинг/плагины на Python.
**Расширение** (extension) — обратный случай: C++-модуль, импортируемый из
Python. Здесь показаны оба направления.

Модель памяти Python: всё — указатели `PyObject*` с подсчётом ссылок. Владение:

- **new reference** — отдаёт владение (уменьшать `Py_DECREF`);
- **borrowed reference** — не отдаёт (не уменьшать).

Инициализация: `Py_Initialize()` (или `Py_InitializeFromConfig` с `PyConfig`),
завершение: `Py_Finalize()`. Выражения исполняет `PyRun_String`/`PyRun_SimpleString`.

## Синтаксис

```cpp
#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyConfig config;
PyConfig_InitPythonConfig(&config);
PyConfig_SetString(&config, &config.home, L"C:/msys64/ucrt64");
Py_InitializeFromConfig(&config);
PyConfig_Clear(&config);

PyObject* global = PyDict_New();
PyDict_SetItemString(global, "__builtins__", PyEval_GetBuiltins());
PyObject* result = PyRun_String("sum(i * i for i in range(5))",
                                Py_eval_input, global, global);
long value = PyLong_AsLong(result);
Py_DECREF(result);
Py_Finalize();

// Rasshireniye: svoystvennyy modul' dlya Python
static PyMethodDef methods[] = {
    {"cpp_double", cpp_double, METH_O, "double an integer"}, {nullptr, nullptr, 0, nullptr}};
static PyModuleDef module = {PyModuleDef_HEAD_INIT, "cpp_bridge", nullptr, -1, methods};
PyObject* m = PyModule_Create(&module);
```

## Подводные камни

- **Утечки ссылок**: каждый `PyRun_String`/`PyModule_Create` возвращает новую
  ссылку — обязателен `Py_DECREF`. Промах = утечка (видно в отладчике ссылок).
- **GIL**: встраивание в многопоточное приложение требует
  `PyGILState_Ensure/Release` вокруг вызовов API.
- **Поиск модулей**: без корректного `home`/`PYTHONHOME` Python не находит
  `Lib/` и печатает предупреждения; здесь `home` задаётся явно.
- **Версия ABI**: расширения компилируются под конкретный Python (`3.12`), а
  стабильный ABI (`Py_LIMITED_API`/abi3) ограничивает доступный API ради
  переносимости.
- **Исключения Python**: возврат `NULL` = ошибка; смотрите
  `PyErr_Occurred()`/`PyErr_Print()`, а не полагайтесь на код возврата.
- **Порядок освобождения**: уменьшайте ссылки до `Py_Finalize`.

## Задача

Встроить CPython: вычислить `sum(i*i for i in range(5))` (30) на стороне Python
и вызвать C++-функцию `cpp_double(21)` (42) из Python через собственный модуль
`cpp_bridge`. Без заголовков Python — воспроизвести те же числа симуляцией.

## Где применяется

- Приложения со скриптингом (Blender, GIMP), автоматизация и расчёты, серверы
  с плагинами на Python, ML-пайплайны, тестовые фреймворки.

## Проверка

С реальным Python: `python version = 3.12.9 ...`,
`python sum of squares = 30`, `python -> cpp_double(21) = 42`,
`python embedding done`. Без заголовков: `Python.h not found; simulated
embedding`, далее `simulated ... = 30/42`. Код возврата `0` при совпадении,
иначе `1`.

## Решение

См. `ex05_python_embedding.cpp`. Реальная ветка включается через
`__has_include(<Python.h>)`; сборка — с
`-IC:/msys64/ucrt64/include/python3.12 -LC:/msys64/ucrt64/lib -lpython3.12`
(msys2 ucrt64, Python 3.12).
