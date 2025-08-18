@echo off

echo Starting local server for Crimson WASM build...
echo.
echo Navigate to:
echo   http://localhost:8000/Debug for a Debug build
echo   http://localhost:8000/Release for a Release build
echo.
echo Press Ctrl+C to stop the server
echo.

cd /d "%~dp0..\..\build-wasm\bin"

python -m http.server 8000 2>nul || python -m SimpleHTTPServer 8000 2>nul || npx serve -l 8000 .

pause
