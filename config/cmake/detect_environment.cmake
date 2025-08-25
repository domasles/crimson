if(CMAKE_VERSION VERSION_LESS "3.16")
    message(WARNING "CMake 3.16 or higher is recommended for best experience. Current version: ${CMAKE_VERSION}")
endif()

set(DEV_ENV "unknown")

if(DEFINED ENV{VSCODE_PID} OR CMAKE_EXPORT_COMPILE_COMMANDS)
    set(DEV_ENV "vscode")
elseif(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(DEV_ENV "visual_studio")
elseif(DEFINED ENV{CLION_IDE})
    set(DEV_ENV "clion")
endif()

message(STATUS "Detected development environment: ${DEV_ENV}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are not recommended. Consider using a separate build directory.")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified, defaulting to Debug")

    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands for IDEs" FORCE)
