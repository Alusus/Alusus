@echo off
setlocal enabledelayedexpansion

REM We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).

REM Set the script directory.
for %%i in ("%~dp0.") do set "ALUSUS_BUILD_SCRIPT_DIR=%%~fi"

REM Give users an option to select a different Python 3 interpreter.
if "%ALUSUS_PYTHON_INTERP%"=="" ( 
    set ALUSUS_PYTHON_INTERP=python
    where python3 >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        set ALUSUS_PYTHON_INTERP=python3
    )
) else (
    set ALUSUS_PYTHON_INTERP=%ALUSUS_PYTHON_INTERP%
)

!ALUSUS_PYTHON_INTERP! "!ALUSUS_BUILD_SCRIPT_DIR!\BuildSrc\create_patch.py" %*
exit /b !ERRORLEVEL!
