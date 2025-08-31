#!/bin/sh

set -e
cd /usr/share/nginx/html

# Find hash-named HTML
HASHED_HTML=$(ls *.html | head -n 1)

# Symlink to index.html
ln -sf "$HASHED_HTML" index.html
nginx -g "daemon off;"
