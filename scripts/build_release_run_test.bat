@echo off

call build_release.bat

if %errorlevel% NEQ 0  (
    goto errortag
)

call run_tests.bat
exit 0


:errortag
echo Compilation failed. Aborting tests...
exit /B %errorlevel%
