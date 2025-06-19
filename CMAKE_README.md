# Crimson Game Engine - CMake Build System

This project has been migrated from Premake to CMake for better integration with modern development tools.

## Prerequisites

- CMake 3.16 or higher
- Visual Studio 2022 (Windows) or GCC (Linux)
- VS Code with recommended extensions (C/C++, CMake Tools)

## Quick Start

### Windows
1. Run `GenerateProjects-Windows-CMake.bat` to generate Visual Studio project files
2. Open `Crimson.sln` in Visual Studio (copied to project root), or use VS Code with CMake extension
3. Build intermediate files are in `build/` directory, but solution is conveniently in root

### Linux
1. Run `GenerateProjects-Linux-CMake.sh` to generate Makefiles
2. Use VS Code with CMake extension for development

## VS Code Development

The project is configured for seamless VS Code development:

1. Open the project folder in VS Code
2. Install recommended extensions when prompted
3. CMake will auto-configure on first open
4. Use **F5** to build and debug
5. Use **Ctrl+Shift+P** → "CMake: Build" to build manually

## Project Structure

- `engine/` - Core game engine (static library)
- `games/crimson/` - Main game project (shared library)  
- `launcher/` - Game launcher executable
- `build/` - CMake build directory (generated)

## Build Configurations

- **Debug** - Debug symbols, no optimization
- **Release** - Optimized build for distribution

## Dependencies

- **SDL3** - Graphics and input (built from source)
- **SDL3_image** - Image loading (built from source, minimal backends)
- **nlohmann/json** - JSON parsing (header-only)

All SDL dependencies are built as static libraries from source code included in the project.
This ensures compatibility and eliminates the need for external DLL files.

## Build Status

✅ **Successfully migrated to CMake**
- SDL3 and SDL3_image build from source as static libraries
- No external dependencies or DLL files required
- Supports both Debug and Release configurations
- Full VSCode integration with F5 debugging
- **Visual Studio file organization**: Files grouped by folders (src/, include/, utils/, etc.)
- **Solution convenience**: Crimson.sln copied to project root for easy access

## Building

### Command Line
```bash
# Configure
cmake -B build -G "Visual Studio 17 2022" -A x64

# Build Debug
cmake --build build --config Debug

# Build Release  
cmake --build build --config Release
```

### VS Code
- **Ctrl+Shift+P** → "CMake: Configure"
- **Ctrl+Shift+P** → "CMake: Build" 
- **F5** - Build and Debug

## Output

Built executables and libraries are placed in:
- `build/bin/Debug/` or `build/bin/Release/`
- Game assets are automatically copied to the game directory
