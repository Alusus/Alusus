@echo off
setlocal enabledelayedexpansion

if "%MSYS_DIR%"=="" (
    set MSYS_DIR=c:\msys64
) else (
    set MSYS_DIR=%MSYS_DIR%
)

if "%PATH:)=^)%"=="" (
    set Path=!MSYS_DIR!\usr\bin;!MSYS_DIR!\usr\lib
) else (
    set Path=!MSYS_DIR!\usr\bin;!MSYS_DIR!\usr\lib;%PATH:)=^)%
)
if "%LIBRARY_PATH%"=="" (
    set LIBRARY_PATH=!MSYS_DIR!\usr\lib;
) else (
    set LIBRARY_PATH=!MSYS_DIR!\usr\lib;%LIBRARY_PATH%
)

if "%MSYS_SHELL%" == "" (
    set MSYS_SHELL=C:\Windows\System32\cmd.exe
) else (
    set MSYS_SHELL=%MSYS_SHELL%
)

!MSYS_SHELL! %*

endlocal
