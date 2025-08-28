# Crimson outBreak Game Dockerfile
FROM emscripten/emsdk:latest AS builder

# Set up working directory
WORKDIR /app

# Install Ninja build system
RUN apt update && apt install -y ninja-build

# Clone the git repository with submodules
RUN git clone --depth 1 --shallow-submodules --recurse-submodules https://github.com/domasles/crimson .

# Configure CMake for WebAssembly build
RUN emcmake cmake -B build-wasm -G "Ninja Multi-Config"

# Build only the outBreak game target
RUN cmake --build build-wasm --target outBreak --config Release

# Production stage with Nginx
FROM nginx:alpine

# Copy built WebAssembly game
COPY --from=builder /app/build-wasm/bin/Release/outBreak/ /usr/share/nginx/html/

# Copy nginx configuration
COPY games/outBreak/docker/nginx.conf /etc/nginx/conf.d/default.conf
COPY games/outBreak/docker/entrypoint.sh /entrypoint.sh

# Expose the web app port
EXPOSE 80

# Make entrypoint script executable
RUN chmod +x /entrypoint.sh

# Run the web app
ENTRYPOINT ["/entrypoint.sh"]
