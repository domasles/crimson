# Environment detection and validation

# Validate CMake version (with warning, not error for compatibility)
if(CMAKE_VERSION VERSION_LESS "3.16")
    message(WARNING "CMake 3.16 or higher is recommended for best experience. Current version: ${CMAKE_VERSION}")
endif()

# Detect development environment
set(CRIMSON_DEV_ENV "unknown")

if(DEFINED ENV{VSCODE_PID} OR CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CRIMSON_DEV_ENV "vscode")
elseif(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CRIMSON_DEV_ENV "visual_studio")
elseif(DEFINED ENV{CLION_IDE})
    set(CRIMSON_DEV_ENV "clion")
endif()

message(STATUS "Detected development environment: ${CRIMSON_DEV_ENV}")

# Validate build directory (warning only for compatibility)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are not recommended. Consider using a separate build directory.")
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified, defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands for IDEs" FORCE)
