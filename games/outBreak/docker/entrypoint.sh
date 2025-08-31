#!/bin/sh

set -e
cd /usr/share/nginx/html

# Read hash from file
if [ -f build_hash.txt ]; then
  HASHED_HTML=$(cat build_hash.txt).html
else
  echo "build_hash.txt not found, defaulting to index.html"
  HASHED_HTML="index.html"
fi

# Symlink it
ln -sf "$HASHED_HTML" index.html

# Start nginx
nginx -g "daemon off;"
