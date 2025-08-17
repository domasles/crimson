cmake_minimum_required(VERSION 3.16)

project(Crimson
    LANGUAGES C CXX
    VERSION 1.0.0
    DESCRIPTION "Crimson Game Engine"
)

# ===== CMAKE MODULE PATH =====
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config/cmake")

# ===== ENVIRONMENT DETECTION =====
include(detect_environment)

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
if(WASM)
    # For WASM builds, respect Debug/Release directories
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
else()
    # For native builds
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
endif()

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
# Use generator expressions to properly handle multi-config generators
add_compile_definitions(
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Debug>:GAME_DEBUG>
    $<$<CONFIG:Debug>:LAUNCHER_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
    $<$<CONFIG:Release>:GAME_RELEASE>
    $<$<CONFIG:Release>:LAUNCHER_RELEASE>
)

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
                
                # Configure WASM build using emcmake with multi-config support
                message(STATUS "Configuring WASM build...")
                execute_process(
                    COMMAND "${EMCMAKE_CMD}" cmake -B build-wasm -S . -G "Ninja Multi-Config"
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

# ===== VS CODE CONFIGURATION =====
include(configure_vscode)
