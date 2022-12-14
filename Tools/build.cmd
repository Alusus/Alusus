@echo off
REM We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).
set SCRIPT_DIR=%~dp0
python "%SCRIPT_DIR%\BuildSrc\alusus_install_deps.py" &&^
python "%SCRIPT_DIR%\BuildSrc\alusus_build.py" %*
exit /b %errorlevel%
