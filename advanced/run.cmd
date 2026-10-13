@echo off
rem Zapusk kursa advanced: peredaet argumenty v run.ps1.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0run.ps1" %*
