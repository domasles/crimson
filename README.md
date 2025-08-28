![Crimson Logo](./CrimsonLogo.svg)

# Crimson

A simple, powerful 2D game engine designed for learning game development fundamentals. Built with modern C++ and focused on providing an easy-to-understand API that doesn't hide the underlying architecture.

## Why Crimson?

**Focus**: Simplicity and learning game development at a lower level with an intuitive API.

**Purpose**: This project was born out of passion to understand low-level game engine architecture myself, and to provide an open library for the community, requiring no licensing fees and providing true cross-platform compatibility.

## Key Features

### Engine Foundation
- **SDL3-Powered Graphics & Audio**: Modern, hardware-accelerated rendering and audio mixing.
- **LDtk Level Editor Integration**: Professional visual level design with automatic collision generation.
- **Component-Based Architecture**: Clean ECS pattern with specialized, reusable components.
- **Advanced Collision System**: Physics-accurate collision detection with shape-based collision primitives.
- **JSON-Driven Configuration**: Maps, settings, input bindings - everything configurable via JSON.
- **Cross-Platform Input**: Unified input handling.
- **Modular Architecture**: Easily extendable and maintainable codebase.
- **Scene Management**: Efficient handling of game scenes and transitions.
- **Texture and audio support**: Comprehensive support for various image and audio formats.

### Multi-Game Framework
- **One Launcher, Multiple Games**: Professional game selection interface with metadata support.
- **Hot-Swappable Games**: Games built as dynamic libraries for rapid development iteration.
- **Per-Game Asset Management**: Isolated assets and configurations per game project.
- **Automatic Game Discovery**: JSON manifest-based game detection and loading.

### Build & Deployment
- **Universal Platform Support**: Windows, Linux, macOS (native) + WebAssembly (web).
- **Modern CMake Integration**: Industry-standard build system with full IDE support.
- **Development Automation**: Build and deployment scripts for streamlined workflows.
- **Everything included through Git submodules**: SDL3, JSON library, WxWidgets and any other dependencies that might become required in the future.

### Coming Soon
- **UI System**: Immediate-mode GUI for game interfaces and debug tools.
- **Comprehensive Documentation**: API reference and game development tutorials.
- **Multiplayer Support**: Networking foundations for online gameplay (long-term goal).

## Quick Start

### Prerequisites
- **CMake 3.16+**
- **Visual Studio 2022** (Windows), **GCC 7+** (Linux), or **Xcode** (macOS)
- **Git** with submodules support

### Setup
```bash
# Clone with all dependencies
git clone ----recurse-submodules https://github.com/domasles/crimson.git
cd crimson

# If you forgot --recurse-submodules, run this:
git submodule update --init --recursive
```

### Development Scripts
Use the automation scripts in the `scripts/` directory (running these works for any platform available):
```bash
# Generate or regenerate build files (cross-platform)
./scripts/projects/GenerateProjects

# Generate a game from sample
./scripts/projects/GenerateGame

# Serve WebAssembly builds locally
./scripts/wasm/ServeWasm
```

### Build Options
```bash
# Native builds (Windows/Linux/macOS)
cmake --build build-native --config Debug

# WebAssembly builds
cmake --build build-wasm --config Debug

# Build specific game only
cmake --build build-native --target crimson --config Debug
cmake --build build-native --target outBreak --config Debug
```

## Daily Development Workflow

### VS Code (Recommended)
1. **Open workspace**: `code .` in project root.
2. **Auto-configure**: CMake Tools extension detects project automatically.
3. **Build & Debug**: Press **F5** - builds and launches with debugger attached.
4. **Build only**: **Ctrl+Shift+P** → `Tasks: Run Task` → `CMake: Build`.
5. **Clean build**: **Ctrl+Shift+P** → `Tasks: Run Task` → `CMake: Clean`.

### Visual Studio (Windows)
1. **Open solution**: `build-native\Crimson.sln`.
2. **Build & Run**: Press **F5**.
3. **Configuration**: Switch between Debug/Release in toolbar.

### Command Line Development
```bash
# Quick development builds
cmake --build build-native --config Debug --parallel

# Release builds
cmake --build build-native --config Release --parallel

# Clean everything
cmake --build build-native --target clean
```

### Game-Specific Development
```bash
# Work on individual games
cmake --build build-native --target crimson
cmake --build build-native --target outBreak
cmake --build build-native --target launcher   # Game launcher

# WebAssembly deployment
cmake --build build-wasm --target crimson --config Release
cmake --build build-wasm --target outBreak
```

## Project Structure

### Games (Framework Convention)
Each game follows a standardized structure that the engine automatically recognizes:

```
games/your-game/
├── src/                    # C++ game logic and entities
├── assets/                 # Auto-loaded game content
│   ├── maps/               # LDtk level files (.ldtk format)
│   ├── tilesets/           # Sprite sheets and texture files
│   └── ...                 # Other game assets
├── config/                 # Auto-loaded JSON configurations
│   ├── Manifest.json       # Game metadata for launcher
│   ├── InputActions.json   # Input bindings
│   └── ...                 # Other configuration files
└── CMakeLists.txt          # Build configuration
```

**Asset Loading**: Files in `assets/` and `config/` directories are automatically copied to build output and loaded by the engine.

**LDtk Integration**: Place `.ldtk` files in `assets/maps/` for automatic level loading with collision detection.

### Launcher Architecture
```
launcher/
├── src/
│   ├── utils/        # Game detection and loading utilities
│   ├── include/      # Launcher-specific headers that mimic structure from src/
│   └── pch/          # Precompiled headers
└── ...               # Cross-platform launcher implementation
```

The launcher automatically scans the `games/` directory and loads game manifests for a unified game selection experience.

### Engine Foundation
```
engine/
├── src/
│   ├── include/      # Public API headers that mimic structure from src/
│   ├── components/   # ECS component implementations
│   │   ├── TransformComponent    # Position, rotation, scale
│   │   ├── TextureComponent      # Sprite rendering
│   │   ├── CollisionComponent    # Physics collision
│   │   ├── InputComponent        # Input handling
│   │   └── ...
│   ├── collisions/   # Physics and collision detection
│   ├── utils/        # Core utilities (math, rendering, I/O)
│   └── ...           # Engine implementation
└── ...
```

## Creating Your First Game

### Game Setup
1. **Create game directory**: `games/my-game/`.
2. **Add CMakeLists.txt**: Link against engine library.
3. **Create manifest.json**: Game metadata for launcher.
4. **Set up standard directories**: `src/`, `assets/`, `config/`.
5. **Include Engine.h**: Add `#include <Engine.h>` to your source files.
6. **Enjoy creating with Crimson!**

### Level Design with LDtk
1. **Install LDtk**: Download from [ldtk.io](https://ldtk.io).
2. **Create levels**: Design levels visually with tiles and entities.
3. **Export to assets/maps/**: Save `.ldtk` files in your game's maps directory.
4. **Automatic integration**: Engine loads levels and generates collision boundaries.

## Build Configuration

### Platform Support Status
- **Windows**: Fully supported and tested.
- **Linux**: Fully supported and tested.
- **macOS**: Supported but not extensively tested.
- **WebAssembly**: Fully supported for web deployment.

## Dependencies & Technology

### Core Libraries (Built from Source)
- **[SDL3](https://github.com/libsdl-org/SDL)** - Modern graphics, audio, input, and windowing
- **[SDL3_image](https://github.com/libsdl-org/SDL_image)** - Image loading (PNG, JPG, BMP, GIF)
- **[SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)** - Audio mixing and playback
- **[WxWidgets](https://github.com/wxWidgets/wxWidgets)** - Cross-platform GUI toolkit

### Header-Only Libraries
- **[Json](https://github.com/nlohmann/json)** - JSON parsing throughout engine

**Zero Installation Required**: All dependencies are included as submodules and built automatically.

## Troubleshooting

### Common Issues

**Most issues** can be resolved by simply regenerating the projects, using `./scripts/projects/GenerateProjects`. However, some problems may require additional steps.

#### "Cannot find SDL3.dll" Error
```bash
# Regenerate and rebuild to ensure libraries are copied
./scripts/projects/GenerateProjects
cmake --build build-native --config Debug
```

#### Build Fails After Git Pull
```bash
# Update submodules, regenerate projects and clean build
git submodule update --init --recursive

./scripts/projects/GenerateProjects

cmake --build build-native --target clean
cmake --build build-native
```

#### WebAssembly Build Issues
```bash
# Ensure Emscripten is installed and activated
./vendor/emsdk/emsdk install latest
./vendor/emsdk/emsdk activate latest
```

### Performance Tips
- **Use Debug builds** for development (faster compilation).
- **Use Release builds** only for final deployment.
- **Build specific targets** instead of building everything.
- **Enable parallel builds** with `--parallel` flag.

## Best Practices

### Game Development
- **Start with examples**: Study existing games in `games/` directory.
- **Use LDtk for levels**: Visual level design is faster and more intuitive.
- **Organize assets**: Keep tilesets and maps properly organized.
- **JSON for configuration**: Use JSON files for game settings and data.

### Code Organization
- **Component-based design**: Build functionality through composition.
- **Keep engine API simple**: Focus on game logic, let engine handle complexity.
- **Use precompiled headers**: Include frequently used headers in `pch.h`.
- **Platform-agnostic code**: Engine abstracts platform differences.

### Development Workflow
1. **Make small, frequent commits** with clear messages.
2. **Test on target platforms** before major releases.
3. **Use branches** for experimental features.
4. **Profile performance** in Release builds only.

## External Submodule Dependencies

- **[SDL3](https://github.com/libsdl-org/SDL)**: For graphics, audio, and input handling
- **[SDL3_image](https://github.com/libsdl-org/SDL_image)**: For image loading (PNG, JPG, BMP, GIF)
- **[SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)**: For audio mixing and playback
- **[WxWidgets](https://github.com/wxWidgets/wxWidgets)**: For cross-platform GUI
- **[Json](https://github.com/nlohmann/json)**: For JSON parsing throughout engine
- **[Emscripten](https://github.com/emscripten-core/emsdk.git)**: For WebAssembly support

---

**Happy game development with Crimson!**

*Built with passion for learning and community. Free forever, no strings attached.*
