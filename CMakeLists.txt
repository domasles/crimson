cmake_minimum_required(VERSION 3.16)

project(Crimson
    LANGUAGES C CXX
    VERSION 1.0.0
    DESCRIPTION "Crimson Game Engine"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config/cmake")
include(detect_environment)

if(EMSCRIPTEN)
    set(WASM ON)
else()
    set(WASM OFF)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

if(WASM)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    foreach(config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${config} config_upper)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib/${config})
    endforeach()
endif()

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

add_compile_definitions(
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Debug>:GAME_DEBUG>
    $<$<CONFIG:Debug>:LAUNCHER_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
    $<$<CONFIG:Release>:GAME_RELEASE>
    $<$<CONFIG:Release>:LAUNCHER_RELEASE>
)

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

if(WASM)
    set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as shared library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build SDL3 as static library" FORCE)
    set(BUILD_SHARED_LIBS OFF)
else()
    set(SDL_SHARED ON CACHE BOOL "Build SDL3 as shared library")
    set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as static library")
    set(BUILD_SHARED_LIBS ON)
endif()

set(SDL_TESTS OFF CACHE BOOL "Build SDL3 tests")
set(SDL_EXAMPLES OFF CACHE BOOL "Build SDL3 examples")

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

set(SDLMIXER_VENDORED ON CACHE BOOL "Use vendored libraries for SDL3_mixer")
set(SDL3MIXER_ENABLE_MP3_MPG123 ON CACHE BOOL "Enable MP3 support via mpg123")
set(SDL3MIXER_ENABLE_OGG ON CACHE BOOL "Enable Ogg vorbis support (stb or libvorbis)")
set(SDL3MIXER_ENABLE_FLAC ON CACHE BOOL "Enable FLAC support (dr_flac or libflac)")
set(SDL3MIXER_MIDI OFF CACHE BOOL "Disable MIDI support for SDL3_mixer")
set(SDL3MIXER_OPUS OFF CACHE BOOL "Disable Opus support for SDL3_mixer")

add_subdirectory(vendor/sdl3)
add_subdirectory(vendor/sdl3-image)
add_subdirectory(vendor/sdl3-mixer)

add_subdirectory(engine)
add_subdirectory(launcher)

file(GLOB children RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/games" "${CMAKE_CURRENT_SOURCE_DIR}/games/*")

foreach(child ${children})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/games/${child}")
        add_subdirectory("games/${child}")
    endif()
endforeach()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT launcher)

option(CRIMSON_BUILD_NATIVE "Enable native host build (Win/Linux/Mac)" ON)
option(CRIMSON_BUILD_WASM "Enable WebAssembly build (Emscripten)" ON)

if(CRIMSON_BUILD_NATIVE)
    message(STATUS "Configuring Native build...")
endif()

if(CRIMSON_BUILD_WASM)
    message(STATUS "Configuring WASM build...")

    if(WIN32)
        set(EMSDK_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/emsdk.bat")
        set(EMCMAKE_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/upstream/emscripten/emcmake.bat")
    else()
        set(EMSDK_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/emsdk")
        set(EMCMAKE_CMD "${CMAKE_SOURCE_DIR}/vendor/emsdk/upstream/emscripten/emcmake")
    endif()

    if(EXISTS "${EMSDK_CMD}")
        message(STATUS "Found Emscripten SDK, setting up WASM build...")

        set(WASM_BUILD_DIR "${CMAKE_SOURCE_DIR}/build-wasm")
        file(MAKE_DIRECTORY "${WASM_BUILD_DIR}")

        execute_process(
            COMMAND "${EMSDK_CMD}" install latest
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/emsdk"
        )
        execute_process(
            COMMAND "${EMSDK_CMD}" activate latest
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/emsdk"
        )

        execute_process(
            COMMAND "${EMCMAKE_CMD}" cmake -B "${WASM_BUILD_DIR}" -S "${CMAKE_SOURCE_DIR}" -G "Ninja Multi-Config"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE WASM_CONFIG_RESULT
        )
    else()
        message(WARNING "Emscripten SDK not found. WASM build skipped.")
    endif()
endif()

include(configure_vscode)
