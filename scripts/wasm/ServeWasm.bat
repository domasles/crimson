@echo off

echo Starting local server for Crimson WASM build...
echo.
echo Navigate to: http://localhost:8000/
echo.
echo Press Ctrl+C to stop the server
echo.

cd /d "%~dp0build-wasm\bin"

python -m http.server 8000 2>nul || python -m SimpleHTTPServer 8000 2>nul || npx serve -l 8000 .

pause
