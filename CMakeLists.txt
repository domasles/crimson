cmake_minimum_required(VERSION 3.16)

project(Crimson
    LANGUAGES C CXX
    VERSION 1.0.0
    DESCRIPTION "Crimson Game Engine"
)

# ===== WASM BUILD DETECTION =====
if(EMSCRIPTEN)
    set(WASM ON)
    message(STATUS "Building for WebAssembly/Emscripten")
else()
    set(WASM OFF)
    message(STATUS "Building for native platform")
endif()

# ===== GLOBAL SETTINGS =====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# ===== OUTPUT DIRECTORIES =====
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Multi-config generators (Visual Studio, Xcode)
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib/${config})
endforeach()

# ===== PLATFORM DEFINITIONS =====
if(WASM)
    add_compile_definitions(
        ENGINE_PLATFORM_EMSCRIPTEN
        GAME_PLATFORM_EMSCRIPTEN
        LAUNCHER_PLATFORM_EMSCRIPTEN
    )
elseif(WIN32)
    add_compile_definitions(
        ENGINE_PLATFORM_WINDOWS
        GAME_PLATFORM_WINDOWS
        LAUNCHER_PLATFORM_WINDOWS
    )
elseif(APPLE)
    add_compile_definitions(
        ENGINE_PLATFORM_MACOS
        GAME_PLATFORM_MACOS
        LAUNCHER_PLATFORM_MACOS
    )
elseif(UNIX)
    add_compile_definitions(
        ENGINE_PLATFORM_LINUX
        GAME_PLATFORM_LINUX
        LAUNCHER_PLATFORM_LINUX
    )
endif()

# ===== BUILD CONFIGURATION DEFINITIONS =====
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(
        ENGINE_DEBUG
        GAME_DEBUG
        LAUNCHER_DEBUG
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(
        ENGINE_RELEASE
        GAME_RELEASE
        LAUNCHER_RELEASE
    )
endif()

# ===== COMPILER SETTINGS =====
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/utf-8 /MP)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fPIC)
    if(NOT APPLE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/../..")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    else()
        set(CMAKE_INSTALL_RPATH "@executable_path:@executable_path/../..")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()
endif()

# ===== FUNCTION TO GENERATE VSCODE CONFIG =====
function(generate_vscode_config)
    # Create .vscode directory
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/.vscode")
    
    # Detect platform for configuration
    if(WIN32)
        set(PROBLEM_MATCHER "$msvc")
        set(DEBUGGER_TYPE "cppvsdbg")
        set(EXECUTABLE_PATH "\${workspaceFolder}/build/bin/Debug/launcher.exe")
        set(WORKING_DIR "\${workspaceFolder}/build/bin/Debug")
        set(GENERATOR "Visual Studio 17 2022")
        set(PREFERRED_GENERATORS "Visual Studio 17 2022" "Visual Studio 16 2019" "Ninja")
        set(COMPILER_PATH "")
        if(MSVC)
            get_filename_component(COMPILER_PATH "${CMAKE_CXX_COMPILER}" ABSOLUTE)
        endif()
        if(NOT COMPILER_PATH OR NOT EXISTS "${COMPILER_PATH}")
            set(COMPILER_PATH "cl.exe")
        endif()
    elseif(APPLE)
        set(PROBLEM_MATCHER "$gcc")
        set(DEBUGGER_TYPE "cppdbg")
        set(EXECUTABLE_PATH "\${workspaceFolder}/build/bin/launcher")
        set(WORKING_DIR "\${workspaceFolder}/build/bin")
        set(GENERATOR "Xcode")
        set(PREFERRED_GENERATORS "Xcode" "Unix Makefiles" "Ninja")
        set(COMPILER_PATH "/usr/bin/clang++")
    else()
        set(PROBLEM_MATCHER "$gcc")
        set(DEBUGGER_TYPE "cppdbg")
        set(EXECUTABLE_PATH "\${workspaceFolder}/build/bin/launcher")
        set(WORKING_DIR "\${workspaceFolder}/build/bin")
        set(GENERATOR "Unix Makefiles")
        set(PREFERRED_GENERATORS "Unix Makefiles" "Ninja")
        set(COMPILER_PATH "/usr/bin/gcc")
    endif()
    
    # Generate tasks.json from template
    file(READ "${CMAKE_SOURCE_DIR}/config/vscode/tasks.json.in" TASKS_TEMPLATE)
    string(REPLACE "@PROBLEM_MATCHER@" "${PROBLEM_MATCHER}" TASKS_CONTENT "${TASKS_TEMPLATE}")
    file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/tasks.json" "${TASKS_CONTENT}")

    # Generate launch.json from template
    file(READ "${CMAKE_SOURCE_DIR}/config/vscode/launch.json.in" LAUNCH_TEMPLATE)
    string(REPLACE "@DEBUGGER_TYPE@" "${DEBUGGER_TYPE}" LAUNCH_CONTENT "${LAUNCH_TEMPLATE}")
    string(REPLACE "@EXECUTABLE_PATH@" "${EXECUTABLE_PATH}" LAUNCH_CONTENT "${LAUNCH_CONTENT}")
    string(REPLACE "@WORKING_DIR@" "${WORKING_DIR}" LAUNCH_CONTENT "${LAUNCH_CONTENT}")
    file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/launch.json" "${LAUNCH_CONTENT}")

    # Generate settings.json from template
    string(REPLACE ";" "\",\n        \"" GENERATORS_JSON "${PREFERRED_GENERATORS}")
    set(PREFERRED_GENERATORS_JSON "[
        \"${GENERATORS_JSON}\"
    ]")
    file(READ "${CMAKE_SOURCE_DIR}/config/vscode/settings.json.in" SETTINGS_TEMPLATE)
    string(REPLACE "@GENERATOR@" "${GENERATOR}" SETTINGS_CONTENT "${SETTINGS_TEMPLATE}")
    string(REPLACE "@PREFERRED_GENERATORS_JSON@" "${PREFERRED_GENERATORS_JSON}" SETTINGS_CONTENT "${SETTINGS_CONTENT}")
    file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/settings.json" "${SETTINGS_CONTENT}")

    # Generate c_cpp_properties.json from template
    file(READ "${CMAKE_SOURCE_DIR}/config/vscode/c_cpp_properties.json.in" CPP_TEMPLATE)
    string(REPLACE "@COMPILER_PATH@" "${COMPILER_PATH}" CPP_CONTENT "${CPP_TEMPLATE}")
    file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json" "${CPP_CONTENT}")

    message(STATUS "Generated VSCode configuration files in .vscode/")
endfunction()

# ===== SDL3 CONFIGURATION =====
if(WASM)
    # Static linking for WASM
    set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as shared library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build SDL3 as static library" FORCE)
    set(BUILD_SHARED_LIBS OFF)
else()
    # Shared linking for native platforms
    set(SDL_SHARED ON CACHE BOOL "Build SDL3 as shared library")
    set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as static library")
    set(BUILD_SHARED_LIBS ON)
endif()

set(SDL_TESTS OFF CACHE BOOL "Build SDL3 tests")
set(SDL_EXAMPLES OFF CACHE BOOL "Build SDL3 examples")

# ===== SDL3_IMAGE CONFIGURATION =====
set(SDLIMAGE_VENDORED OFF CACHE BOOL "Use vendored third-party libraries")
set(SDLIMAGE_TESTS OFF CACHE BOOL "Build SDL3_image tests")
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "Build SDL3_image samples")
set(SDLIMAGE_AVIF OFF CACHE BOOL "Support loading AVIF images")
set(SDLIMAGE_TIF OFF CACHE BOOL "Support loading TIF images")
set(SDLIMAGE_WEBP OFF CACHE BOOL "Support loading WEBP images")
set(SDLIMAGE_JXL OFF CACHE BOOL "Support loading JXL images")
set(SDLIMAGE_PNG ON CACHE BOOL "Support loading PNG images")
set(SDLIMAGE_JPG ON CACHE BOOL "Support loading JPG images")
set(SDLIMAGE_BMP ON CACHE BOOL "Support loading BMP images")
set(SDLIMAGE_GIF ON CACHE BOOL "Support loading GIF images")
set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "Use stb_image for loading JPEG and PNG files")

# ===== SDL3_MIXER CONFIGURATION =====
set(SDLMIXER_VENDORED ON CACHE BOOL "Use vendored libraries for SDL3_mixer")
set(SDL3MIXER_ENABLE_MP3_MPG123 ON CACHE BOOL "Enable MP3 support via mpg123")
set(SDL3MIXER_ENABLE_OGG ON CACHE BOOL "Enable Ogg vorbis support (stb or libvorbis)")
set(SDL3MIXER_ENABLE_FLAC ON CACHE BOOL "Enable FLAC support (dr_flac or libflac)")
set(SDL3MIXER_MIDI OFF CACHE BOOL "Disable MIDI support for SDL3_mixer")
set(SDL3MIXER_OPUS OFF CACHE BOOL "Disable Opus support for SDL3_mixer")

# ===== DEPENDENCIES =====
add_subdirectory(vendor/sdl3)
add_subdirectory(vendor/sdl3-image)
add_subdirectory(vendor/sdl3-mixer)

# ===== PROJECT MODULES =====
add_subdirectory(engine)
add_subdirectory(launcher)

file(GLOB children RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/games" "${CMAKE_CURRENT_SOURCE_DIR}/games/*")

foreach(child ${children})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/games/${child}")
        add_subdirectory("games/${child}")
    endif()
endforeach()

# ===== IDE CONFIGURATION =====
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT launcher)

# ===== DUAL BUILD SETUP =====
# Create both native and WASM build directories when generating projects
if(NOT CMAKE_CROSSCOMPILING AND NOT EMSCRIPTEN)
    message(STATUS "Setting up dual build configuration...")
    
    # Create build directory for native builds (if not exists)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/build")
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
        message(STATUS "Created build/ directory for native builds")
    endif()
    
    # Create build-wasm directory and configure it for WASM builds (if not exists)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/build-wasm")
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build-wasm")
        message(STATUS "Created build-wasm/ directory for WASM builds")
        
        # Determine the correct emsdk command based on platform
        if(WIN32)
            set(EMSDK_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/emsdk.bat")
            set(EMCMAKE_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/upstream/emscripten/emcmake.bat")
        else()
            set(EMSDK_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/emsdk")
            set(EMCMAKE_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/upstream/emscripten/emcmake")
        endif()
        
        # Initialize emsdk if it exists
        if(EXISTS "${EMSDK_CMD}")
            message(STATUS "Setting up Emscripten environment...")
            
            # Install and activate latest emsdk
            execute_process(
                COMMAND "${EMSDK_CMD}" install latest
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/emsdk"
                RESULT_VARIABLE EMSDK_INSTALL_RESULT
            )
            
            execute_process(
                COMMAND "${EMSDK_CMD}" activate latest
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/emsdk"
                RESULT_VARIABLE EMSDK_ACTIVATE_RESULT
            )
            
            if(EMSDK_INSTALL_RESULT EQUAL 0 AND EMSDK_ACTIVATE_RESULT EQUAL 0)
                message(STATUS "Emscripten SDK setup completed")
                
                # Configure WASM build using emcmake
                message(STATUS "Configuring WASM build...")
                execute_process(
                    COMMAND "${EMCMAKE_CMD}" cmake -B build-wasm -S .
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                    RESULT_VARIABLE WASM_CONFIG_RESULT
                )
                
                if(WASM_CONFIG_RESULT EQUAL 0)
                    message(STATUS "WASM build configured successfully")
                else()
                    message(WARNING "Failed to configure WASM build (exit code: ${WASM_CONFIG_RESULT})")
                endif()
            else()
                message(WARNING "Failed to set up Emscripten SDK (install: ${EMSDK_INSTALL_RESULT}, activate: ${EMSDK_ACTIVATE_RESULT})")
            endif()
        else()
            message(WARNING "Emscripten SDK not found. WASM builds will not be available.")
        endif()
    endif()
endif()

# Generate VSCode config files
generate_vscode_config()
