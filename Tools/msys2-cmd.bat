@echo off
setlocal
set Path=C:\msys64\usr\bin;C:\msys64\usr\lib;%Path%
@echo on
C:\Windows\System32\cmd.exe %*
@echo off
endlocal
