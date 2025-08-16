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

for %%A in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do (
    for %%B in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
        set "GAME_NAME_UPPER=!GAME_NAME_UPPER:%%A=%%B!"
    )
)
set "GAME_NAME_UPPER=!GAME_NAME:%GAME_NAME%=!"
call :ToUpper GAME_NAME_UPPER "%GAME_NAME%"

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\.."
set "TEMPLATE_DIR=%PROJECT_ROOT%\config\cmake"
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
    
    echo !file! | findstr /r "\.in$" >nul
    if !errorlevel! equ 0 (
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
echo.
echo To build your game:
echo   cmake -B build -S .
echo   cmake --build build

goto :eof

:ToUpper
set "%~1="
for /f "skip=2 delims=" %%I in ('tree "\%~2"') do if not defined %~1 set "%~1=%%~I"
for %%A in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do (
    for %%B in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
        call set "%~1=%%%~1:%%A=%%B%%"
    )
)
goto :eof
