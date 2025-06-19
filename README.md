# Crimson
A 2D top-down action game about saving the planet from a deadly virus that plagues both people and environment surrounding them, resulting in visual artifacts as you go along.
This game comes with a simple to use and expand 2D game engine made using primarily [SDL3](https://github.com/libsdl-org/SDL) and [LDtk](https://github.com/deepnight/ldtk) as a level editor for extra moddability and simplicity creating new games in mind.
The engine comes in a format of a ready to use C++ library and is still considered a WIP project.

## Quick Start

### Prerequisites
- **CMake 3.16+**
- **Visual Studio 2022** (Windows) or **GCC 7+** (Linux)
- **Git** with submodules support
- **VS Code** (recommended) with C/C++ and CMake Tools extensions

### One-Time Setup
```bash
# Clone with all dependencies
git clone --recursive https://github.com/domasles/crimson.git
cd crimson

# Windows: Generate project files
.\GenerateProjects-Windows.bat

# Linux: Generate project files  
./GenerateProjects-Linux.sh
```

### Daily Development Workflow

#### **VS Code (Recommended)**
1. **Open workspace**: `code .` in project root
2. **Auto-configure**: CMake Tools will detect the project automatically
3. **Build & Debug**: Press **F5** - builds and launches with debugger attached
4. **Build only**: **Ctrl+Shift+P** → `Tasks: Run Task` → `CMake: Build Debug`
5. **Clean build**: **Ctrl+Shift+P** → `Tasks: Run Task` → `CMake: Clean`

#### **Visual Studio 2022**
1. **Open solution**: `build\Crimson.sln`
2. **Build & Run**: Press **F5**
3. **Configuration**: Switch between Debug/Release in toolbar
4. **Solution Explorer**: Shows real folder structure (not CMake virtual folders)

#### **Command Line**
```bash
# Build Debug (fast incremental builds)
cmake --build build --config Debug --parallel

# Build Release (optimized)
cmake --build build --config Release --parallel

# Clean everything
cmake --build build --target clean

# Regenerate project files
cmake -B build -S .
```

## Project Architecture

```
crimson/
├── engine/                   # Core game engine (static library)
│   ├── src/
│   │   ├── include/          # Public API headers
│   │   ├── pch/              # Precompiled headers
│   │   ├── utils/            # Utility functions (math, rendering, fs)
│   │   └── *.cpp             # Core engine implementation
├── vendor/                   # Third-party dependencies
│   ├── sdl3/                 # SDL3 (built from source)
│   ├── sdl3-image/           # SDL3_image (built from source)
│   └── json/                 # nlohmann/json (header-only)
├── games/crimson/            # Main game project (shared library)
│   ├── src/
│   │   ├── entities/         # Game entities (Player, etc.)
│   │   ├── scenes/           # Game scenes (Level1, etc.)
│   │   ├── include/          # Game headers
│   │   └── pch/              # Game precompiled headers
│   ├── assets/               # Game assets
│   │   ├── maps/             # LDtk level files
│   │   └── tilesets/         # Tileset images
│   └── config/               # Game configuration
├── launcher/                 # Game launcher (executable)
│   └── src/                  # Launcher source code
├── build/                    # CMake build directory
│   ├── bin/Debug/            # Debug executables & DLLs
│   ├── bin/Release/          # Release executables & DLLs
│   └── Crimson.sln           # Visual Studio solution
└── .vscode/                  # VS Code configuration (auto-generated)
    ├── tasks.json            # Build tasks
    ├── launch.json           # Debug configuration
    ├── settings.json         # Workspace settings
    └── c_cpp_properties.json # IntelliSense config
```

## Development Workflow

### Adding New Code
1. **Engine features**: Add to `engine/src/` with headers in `engine/src/include/`
2. **Game logic**: Add to `games/crimson/src/` with appropriate organization
3. **New games**: Create new folder in `games/` and add CMakeLists.txt

### Debugging
- **VS Code**: Set breakpoints, press **F5**
- **Visual Studio**: Set breakpoints, press **F5** 
- **GDB (Linux)**: `gdb build/bin/Debug/launcher`

### Build Performance
- **Incremental builds**: Only changed files are recompiled
- **Parallel builds**: Uses all CPU cores by default
- **Shared libraries**: SDL3/SDL3_image built once, linked to all targets
- **DLL optimization**: SDL DLLs only copied to launcher, not game modules

### Common Tasks
```bash
# Full clean rebuild
cmake --build build --target clean && cmake --build build --config Debug

# Only rebuild specific target
cmake --build build --target engine --config Debug
cmake --build build --target crimson --config Debug  
cmake --build build --target launcher --config Debug

# Run without building
./build/bin/Debug/launcher.exe  # Windows
./build/bin/Debug/launcher      # Linux
```

## Dependencies & Build System

### Built from Source (Shared Libraries)
- **[SDL3](https://github.com/libsdl-org/SDL)** - Graphics, audio, input, and windowing
- **[SDL3_image](https://github.com/libsdl-org/SDL_image)** - Image loading (PNG, JPG, BMP, GIF support)

### Header-Only Libraries  
- **[nlohmann/json](https://github.com/nlohmann/json)** - JSON parsing

**All dependencies included!** No external installation required. SDL libraries are built as shared libraries (.dll/.so) for faster incremental builds and copied automatically to the launcher directory.

## VS Code Integration

The project includes **auto-generated VS Code configuration**:

### Available Tasks (Ctrl+Shift+P → Tasks: Run Task)
- **CMake: Build Debug** - Build debug configuration (default: **Ctrl+Shift+B**)
- **CMake: Build Release** - Build release configuration  
- **CMake: Clean** - Clean build artifacts
- **CMake: Regenerate** - Regenerate CMake build files

### Launch Configuration
- **Launch Crimson (Debug)** - Build and debug with **F5**
- Automatically builds before launching
- Sets working directory to output folder
- Pre-loads game assets and config

### IntelliSense Features
- **Auto-completion** for engine API
- **Go to Definition** (**F12**)
- **Error highlighting** with MSVC problem matcher
- **Symbol search** (**Ctrl+T**)
- **Include path resolution** for all dependencies

### Build Status & Features

- **Successfully migrated to CMake from Premake**  
- **SDL3 and SDL3_image build from source as shared libraries**  
- **Cross-platform support** (Windows/Linux)  
- **Full VS Code integration** with F5 debugging  
- **Visual Studio Solution Explorer** shows real folder hierarchy  
- **Standard CMake workflow** with solution in `build\Crimson.sln`  
- **Optimized build times** with shared libraries and parallel builds  
- **Auto-generated .vscode files** with platform-specific settings  
- **DLL management** - libraries copied only where needed  

## Advanced Configuration

### CMake Options
```bash
# Use Ninja generator for faster builds (optional)
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug

# Force rebuild of dependencies
cmake -B build -S . --fresh

# Enable verbose build output
cmake --build build --config Debug --verbose
```

### VS Code Settings Override
Edit `.vscode/settings.json` to customize:
- CMake generator preference
- Build directory location  
- IntelliSense configuration
- File associations

### Adding New Dependencies
1. Add submodule to `engine/vendor/`
2. Update `engine/CMakeLists.txt` with new dependency
3. Regenerate project files

## Troubleshooting

### Common Issues & Solutions

#### "Cannot find SDL3.dll" Error
```bash
# Regenerate and rebuild to ensure DLLs are copied
cmake -B build -S .
cmake --build build --config Debug
```

#### IntelliSense Not Working
1. **Ctrl+Shift+P** → `C/C++: Reload IntelliSense Database`
2. **Ctrl+Shift+P** → `CMake: Clean Rebuild`  
3. Restart VS Code

#### Build Fails After Git Pull
```bash
# Update submodules
git submodule update --init --recursive

# Clean and regenerate
cmake --build build --target clean
cmake -B build -S .
cmake --build build --config Debug
```

#### Visual Studio Solution Out of Date
```bash
# Regenerate solution file
.\GenerateProjects-Windows.bat
# Or manually
cmake -B build -S .
```

### Performance Tips
- **Use Ninja generator** for faster incremental builds on Windows
- **Enable parallel builds** (enabled by default)
- **Build only what you need**: Use specific targets instead of building all
- **Use Debug for development**, Release only for final builds

### Git Workflow
```bash
# Always pull with submodules
git pull --recurse-submodules

# When adding new submodules
git submodule add <url> engine/vendor/<name>
git commit -m "Added <name> dependency"
```

## Best Practices

### Code Organization
- **Engine code**: Keep platform-agnostic, well-documented APIs
- **Game code**: Organize by feature (entities, scenes, systems)
- **Headers**: Use include guards, forward declarations when possible
- **PCH**: Include frequently used headers in `pch.h`

### Development Workflow
1. **Make small commits** with clear messages
2. **Test builds** after major changes
3. **Use branches** for experimental features
4. **Profile before optimizing** (Debug builds for development)

### Adding New Games
```cmake
# In games/<newgame>/CMakeLists.txt
add_subdirectory(${CMAKE_SOURCE_DIR}/engine engine)
add_library(<newgame> SHARED ...)
target_link_libraries(<newgame> PRIVATE engine)
```

**Happy coding with Crimson!**
