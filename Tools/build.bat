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

REM Give users an option to select a different path to cache the build script dependencies.
if "%ALUSUS_PYTHONPATH%"=="" (
    set ALUSUS_PYTHONPATH=%HOMEPATH%\.alusus_pythonpath
) else (
    set ALUSUS_PYTHONPATH=%ALUSUS_PYTHONPATH%
)

REM Give users an option to select a different directory to put temporary files.
if "%ALUSUS_TMP_DIR%"=="" (
    set ALUSUS_TMP_DIR=%HOMEPATH%\.alusus_tmp_dir
) else (
    set ALUSUS_TMP_DIR=%ALUSUS_TMP_DIR%
)

REM Create the directory to install the Python dependencies of the build script.
mkdir !ALUSUS_PYTHONPATH! >nul 2>&1

REM Set PYTHONPATH.
if "%PYTHONPATH%"=="" (
    set PYTHONPATH=!ALUSUS_PYTHONPATH!
) else (
    set PYTHONPATH=!ALUSUS_PYTHONPATH!;%PYTHONPATH%
)

REM Install Pip.
!ALUSUS_PYTHON_INTERP! -c "import pkg_resources; pkg_resources.require(""pip==20.1.1"")" >nul 2>&1
if !ERRORLEVEL! NEQ 0 (
    REM Download the "get-pip.py" script.
    !ALUSUS_PYTHON_INTERP! "!ALUSUS_BUILD_SCRIPT_DIR!\BuildSrc\download_get_pip.py" "!ALUSUS_PYTHONPATH!"

    REM Install Pip 20.1.1.
    !ALUSUS_PYTHON_INTERP! "!ALUSUS_PYTHONPATH!\get-pip.py" "pip==20.1.1" --target "!ALUSUS_PYTHONPATH!"

    REM Delete "get-pip.py".
    del "!ALUSUS_PYTHONPATH!\get-pip.py"
)

REM Install all other dependencies.
!ALUSUS_PYTHON_INTERP! -c "import pkg_resources; pkg_resources.require(""ummalqura==2.0.1"")" >nul 2>&1
if !ERRORLEVEL! NEQ 0 (
    !ALUSUS_PYTHON_INTERP! -m pip install ummalqura==2.0.1 --target "!ALUSUS_PYTHONPATH!"
)
!ALUSUS_PYTHON_INTERP! -c "import pkg_resources; pkg_resources.require(""colorama==0.4.3"")" >nul 2>&1
if !ERRORLEVEL! NEQ 0 (
    !ALUSUS_PYTHON_INTERP! -m pip install colorama==0.4.3 --target "!ALUSUS_PYTHONPATH!"
)
!ALUSUS_PYTHON_INTERP! -c "import pkg_resources; pkg_resources.require(""termcolor==1.1.0"")" >nul 2>&1
if !ERRORLEVEL! NEQ 0 (
    !ALUSUS_PYTHON_INTERP! -m pip install termcolor==1.1.0 --target "!ALUSUS_PYTHONPATH!"
)
!ALUSUS_PYTHON_INTERP! -c "import pkg_resources; pkg_resources.require(""wget==3.2"")" >nul 2>&1
if !ERRORLEVEL! NEQ 0 (
    !ALUSUS_PYTHON_INTERP! -m pip install wget==3.2 --target "!ALUSUS_PYTHONPATH!"
)

!ALUSUS_PYTHON_INTERP! "!ALUSUS_BUILD_SCRIPT_DIR!\BuildSrc\build.py" %*
exit /b !ERRORLEVEL!
