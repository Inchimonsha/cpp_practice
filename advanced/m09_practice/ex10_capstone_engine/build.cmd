@echo off
rem Sborka i progon kapstoun-proekta (GCC 16.2, MSYS2 ucrt64).
setlocal
set PATH=C:\msys64\ucrt64\bin;C:\msys64\mingw64\bin;%PATH%
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=C:/msys64/mingw64/bin/mingw32-make.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
if errorlevel 1 exit /b %ERRORLEVEL%
cmake --build build
if errorlevel 1 exit /b %ERRORLEVEL%
ctest --test-dir build --output-on-failure
if errorlevel 1 exit /b %ERRORLEVEL%
build\engine_demo.exe
echo exit=%ERRORLEVEL%
