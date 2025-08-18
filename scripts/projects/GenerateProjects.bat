@echo off
setlocal EnableDelayedExpansion

echo Generating Visual Studio 2022 solution...

cmake -G "Visual Studio 17 2022" -A x64 -B build-native

if %ERRORLEVEL% NEQ 0 (
    echo Failed to generate project files!
    pause
    exit /b 1
)

echo.
echo Project generation completed successfully!
echo Solution file: build-native\Crimson.sln
echo Build files are located in the 'build-native' directory.
echo.
echo To develop:
echo   1. Open build-native\Crimson.sln in Visual Studio, or
echo   2. Open folder in VS Code
echo.
echo To build from command line:
echo   cmake --build build-native --config Debug
echo   cmake --build build-native --config Release
echo.
pause
