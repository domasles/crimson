@echo off
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Usage: %~nx0 ^<game_name^> [display_name]
    echo Example: %~nx0 mygame "My Awesome Game"
    exit /b 1
)

set "GAME_NAME=%~1"
set "GAME_DISPLAY_NAME=%~2"
if "!GAME_DISPLAY_NAME!"=="" set "GAME_DISPLAY_NAME=!GAME_NAME!"

set "GAME_NAME_UPPER=!GAME_NAME!"
call :ToUpper GAME_NAME_UPPER

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\.."
pushd "%PROJECT_ROOT%"
set "PROJECT_ROOT=%CD%"
popd
set "TEMPLATE_DIR=%PROJECT_ROOT%\config\games"
set "GAMES_DIR=%PROJECT_ROOT%\games"
set "TARGET_DIR=%GAMES_DIR%\%GAME_NAME%"

if exist "%TARGET_DIR%" (
    echo Error: Game '%GAME_NAME%' already exists in %TARGET_DIR%
    exit /b 1
)

echo Creating new game: %GAME_NAME% ^(%GAME_DISPLAY_NAME%^)
echo Target directory: %TARGET_DIR%
echo.

mkdir "%TARGET_DIR%" 2>nul

for /r "%TEMPLATE_DIR%" %%f in (*) do (
    set "file=%%f"
    set "rel_path=!file:%TEMPLATE_DIR%\=!"
    set "target_file=%TARGET_DIR%\!rel_path!"
    
    for %%d in ("!target_file!") do mkdir "%%~dpd" 2>nul
    
    set "is_template=0"
    if "!file:~-3!"==".in" set "is_template=1"
    
    if "!is_template!"=="1" (
        set "target_clean=!target_file:.in=!"
        powershell -Command "(Get-Content '!file!') -replace '@GAME_NAME@', '%GAME_NAME%' -replace '@GAME_DISPLAY_NAME@', '%GAME_DISPLAY_NAME%' -replace '@GAME_NAME_UPPER@', '!GAME_NAME_UPPER!' | Set-Content '!target_clean!' -Encoding UTF8"
        echo   Generated: !target_clean!
    ) else (
        copy "!file!" "!target_file!" >nul
        echo   Copied: !target_file!
    )
)

echo.
echo Game '%GAME_NAME%' created successfully!
echo Location: %TARGET_DIR%

mkdir "%TARGET_DIR%\assets" 2>nul

echo.
echo To build your game:
echo   cmake -B build-native -S .
echo   cmake --build build

goto :eof

:ToUpper
setlocal enableextensions enabledelayedexpansion
set "str=!%~1!"
set "upper=ABCDEFGHIJKLMNOPQRSTUVWXYZ"
set "lower=abcdefghijklmnopqrstuvwxyz"
for /L %%i in (0,1,25) do (
    call set "from=%%lower:~%%i,1%%"
    call set "to=%%upper:~%%i,1%%"
    call set "str=%%str:!from!=!to!%%"
)
endlocal & set "%~1=%str%"
goto :eof
