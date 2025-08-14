#!/bin/bash

echo "Starting local server for Crimson WASM build..."
echo ""
echo "Navigate to: http://localhost:8000/launcher.html"
echo ""
echo "Press Ctrl+C to stop the server"
echo ""

cd "$(dirname "$0")/build-wasm/bin"

python3 -m http.server 8000 2>/dev/null || python -m SimpleHTTPServer 8000 2>/dev/null || npx serve -l 8000 .
