@echo off
setlocal
set Path=C:\msys64\mingw64\bin;C:\msys64\mingw64\lib;C:\msys64\mingw64\x86_64-w64-mingw32\bin;C:\msys64\mingw64\x86_64-w64-mingw32\lib;C:\msys64\usr\bin;C:\msys64\usr\lib;C:\Program Files (x86)\Inno Setup 6;%Path%
set MINGW_HOST=x86_64-w64-mingw32
@echo on
C:\Windows\System32\cmd.exe %*
@echo off
endlocal
