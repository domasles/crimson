@echo off
setlocal EnableDelayedExpansion

echo Generating Visual Studio 2022 solution...

:: Generate Visual Studio solution
cmake -G "Visual Studio 17 2022" -A x64 -B build

if %ERRORLEVEL% NEQ 0 (
    echo Failed to generate project files!
    pause
    exit /b 1
)

echo.
echo Project generation completed successfully!
echo Solution file: build\Crimson.sln
echo Build files are located in the 'build' directory.
echo.
echo To develop:
echo   1. Open build\Crimson.sln in Visual Studio, or
echo   2. Open folder in VS Code
echo.
echo To build from command line:
echo   cmake --build build --config Debug
echo   cmake --build build --config Release
echo.
pause
