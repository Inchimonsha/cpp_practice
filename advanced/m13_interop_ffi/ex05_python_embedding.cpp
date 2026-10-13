// m13_interop_ffi/ex05_python_embedding — vstraivanie CPython v C++.
// Real'naya sborka (msys2 ucrt64 python3.12):
//   -Extra '-IC:/msys64/ucrt64/include/python3.12 -LC:/msys64/ucrt64/lib -lpython3.12'
// Bez etih flagov Python.h ne nayden -> rabotaet simulyaciya s tem zhe rezul'tatom.
// Standart: C++23. Zapusk: ./run.ps1 m13_interop_ffi/ex05_python_embedding -Extra '...'
#if defined(__has_include)
#  if __has_include(<Python.h>)
#    define CPP_COURSE_HAVE_PYTHON 1
#  endif
#endif

#include <iostream>
#include <print>

#ifdef CPP_COURSE_HAVE_PYTHON
#  define PY_SSIZE_T_CLEAN
#  include <Python.h>

static PyObject* cpp_double(PyObject*, PyObject* argument) {
    const long value = PyLong_AsLong(argument);
    if (value == -1 && PyErr_Occurred() != nullptr) {
        return nullptr;
    }
    return PyLong_FromLong(2 * value);
}

static PyMethodDef bridge_methods[] = {
    {"cpp_double", cpp_double, METH_O, "double an integer"},
    {nullptr, nullptr, 0, nullptr},
};

static PyModuleDef bridge_module = {
    PyModuleDef_HEAD_INIT, "cpp_bridge", nullptr, -1, bridge_methods, nullptr, nullptr, nullptr, nullptr,
};
#endif

int main() {
#ifdef CPP_COURSE_HAVE_PYTHON
    std::println("python version = {}", Py_GetVersion());

    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    PyConfig_SetString(&config, &config.home, L"C:/msys64/ucrt64");
    Py_InitializeFromConfig(&config);
    PyConfig_Clear(&config);

    PyObject* global = PyDict_New();
    PyDict_SetItemString(global, "__builtins__", PyEval_GetBuiltins());

    PyObject* squares =
        PyRun_String("sum(i * i for i in range(5))", Py_eval_input, global, global);
    const long squares_value = squares != nullptr ? PyLong_AsLong(squares) : -1;
    Py_XDECREF(squares);

    PyObject* module = PyModule_Create(&bridge_module);
    PyDict_SetItemString(global, "cpp_bridge", module);

    PyObject* doubled = PyRun_String("cpp_bridge.cpp_double(21)", Py_eval_input, global, global);
    const long doubled_value = doubled != nullptr ? PyLong_AsLong(doubled) : -1;
    Py_XDECREF(doubled);

    Py_DECREF(module);
    Py_DECREF(global);
    Py_Finalize();

    std::println("python sum of squares = {}", squares_value);
    std::println("python -> cpp_double(21) = {}", doubled_value);

    const bool ok = squares_value == 30 && doubled_value == 42;
    std::cout << "python embedding done\n";
    return ok ? 0 : 1;
#else
    std::println("Python.h not found; simulated embedding");
    long squares_value = 0;
    for (long index = 0; index < 5; ++index) {
        squares_value += index * index;
    }
    const long doubled_value = 2 * 21;
    std::println("simulated sum of squares = {}", squares_value);
    std::println("simulated cpp_double(21) = {}", doubled_value);

    const bool ok = squares_value == 30 && doubled_value == 42;
    std::cout << "python embedding done\n";
    return ok ? 0 : 1;
#endif
}
