@echo off
setlocal EnableDelayedExpansion
SET SCRIPT_DIR=%~dp0

echo Generating Visual Studio 2022 solution and Emscripten build files...

pushd %SCRIPT_DIR%\..\..
    call vendor\emsdk\emsdk install latest
    call vendor\emsdk\emsdk activate latest

    call emcmake cmake -B build-wasm -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=vendor\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake
    call cmake -G "Visual Studio 17 2022" -A x64 -B build-native
popd

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
echo WASM building:
echo   cmake --build build-wasm --config Debug
echo   cmake --build build-wasm --config Release
echo.
pause
