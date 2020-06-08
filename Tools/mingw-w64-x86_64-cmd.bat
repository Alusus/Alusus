@echo off
setlocal enabledelayedexpansion

if "%MSYS_DIR%"=="" (
    set MSYS_DIR=c:\msys64
) else (
    set MSYS_DIR=%MSYS_DIR%
)

if "%INNO_SETUP_DIR%"=="" (
    set INNO_SETUP_DIR=C:\Program Files ^(x86^)\Inno Setup 6
) else (
    set INNO_SETUP_DIR=%INNO_SETUP_DIR%
)

if "%PATH:)=^)%"=="" (
    set Path=!MSYS_DIR!\mingw64\bin;!MSYS_DIR!\mingw64\x86_64-w64-mingw32\bin;!MSYS_DIR!\usr\bin;!MSYS_DIR!\mingw64\lib;!MSYS_DIR!\mingw64\x86_64-w64-mingw32\lib;!MSYS_DIR!\usr\lib;!INNO_SETUP_DIR!
) else (
    set Path=!MSYS_DIR!\mingw64\bin;!MSYS_DIR!\mingw64\x86_64-w64-mingw32\bin;!MSYS_DIR!\usr\bin;!MSYS_DIR!\mingw64\lib;!MSYS_DIR!\mingw64\x86_64-w64-mingw32\lib;!MSYS_DIR!\usr\lib;!INNO_SETUP_DIR!;%PATH:)=^)%
)

if "%LIBRARY_PATH%"=="" (
    set LIBRARY_PATH=!MSYS_DIR!\mingw64\lib;!MSYS_DIR!\mingw64\x86_64-w64-mingw32\lib;!MSYS_DIR!\usr\lib;
) else (
    set LIBRARY_PATH=!MSYS_DIR!\mingw64\lib;!MSYS_DIR!\mingw64\x86_64-w64-mingw32\lib;!MSYS_DIR!\usr\lib;%LIBRARY_PATH%
)

set MINGW_HOST=x86_64-w64-mingw32

if "%MINGW_SHELL%" == "" (
    set MINGW_SHELL=C:\Windows\System32\cmd.exe
) else (
    set MINGW_SHELL=%MINGW_SHELL%
)

!MINGW_SHELL! %*

endlocal
