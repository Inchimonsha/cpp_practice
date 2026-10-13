@echo off
rem Sborka modul'nogo primera (GCC 16.2, MSYS2 ucrt64).
setlocal
set PATH=C:\msys64\ucrt64\bin;%PATH%
g++ -std=c++23 -fmodules-ts math.cppm main.cpp -o app.exe -lstdc++exp
if errorlevel 1 exit /b %ERRORLEVEL%
app.exe
echo exit=%ERRORLEVEL%
