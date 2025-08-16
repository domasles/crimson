#!/bin/bash

echo "Generating build files for Linux..."

# Create build directory for intermediate files only
mkdir -p build

# Generate Makefiles
cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug

if [ $? -ne 0 ]; then
    echo "Failed to generate project files!"
    exit 1
fi

echo ""
echo "Project generation completed successfully!"
echo "Build files are located in the 'build' directory."
echo ""
echo "To build the project:"
echo "  cmake --build build --config Debug"
echo "  cmake --build build --config Release"
echo ""
echo "To develop:"
echo "  Open folder in VS Code"
echo ""
echo "  1. cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "  2. make -j$(nproc)"
echo ""
