@echo off
setlocal enabledelayedexpansion

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

REM Set the the mode to "CC".
set ALUSUS_CC_CXX_BIN_TYPE=CC

!ALUSUS_PYTHON_INTERP! "%ALUSUS_CUSTOM_CC_CXX_PYTHON_SCRIPT_PATH%" %*
exit /b !ERRORLEVEL!
