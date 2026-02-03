# Crimson outBreak Game Dockerfile
FROM emscripten/emsdk:latest AS builder

# Set up working directory
WORKDIR /app

# Install Ninja build system
RUN apt update && apt install -y ninja-build

# Copy the source code
COPY . .

# Configure and run CMake for OutBreak's WebAssembly build
ARG BUILD_HASH=game
ENV BUILD_HASH=$BUILD_HASH

RUN emcmake cmake -B build-wasm -G "Ninja Multi-Config" -DOUTPUT_NAME=$BUILD_HASH \
    && cmake --build build-wasm --target outBreak --config Release \
    && mv build-wasm/bin/Release/outBreak/$BUILD_HASH.html build-wasm/bin/Release/outBreak/index.html

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
