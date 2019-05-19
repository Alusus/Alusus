@echo off
REM We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).
setlocal
set TEMP_PYTHON_INTERP=python
where python3 >nul 2>&1
if %ERRORLEVEL% EQU 0 set TEMP_PYTHON_INTERP=python3
set SCRIPT_DIR=%~dp0
%TEMP_PYTHON_INTERP% "%SCRIPT_DIR%\BuildSrc\build.py" %*
exit /b %ERRORLEVEL%
