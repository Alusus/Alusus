@echo off
REM We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).
setlocal
REM Give users an option to select a different Python 3 interpreter.
if %ALUSUS_PYTHON_INTERP%=="" ( 
    set ALUSUS_PYTHON_INTERP=python
    where python3 >nul 2>&1
    if %ERRORLEVEL% EQU 0 set ALUSUS_PYTHON_INTERP=python3
)
set ALUSUS_BUILD_SCRIPT_DIR=%~dp0
%ALUSUS_PYTHON_INTERP% "%ALUSUS_BUILD_SCRIPT_DIR%\BuildSrc\build.py" %*
exit /b %ERRORLEVEL%
