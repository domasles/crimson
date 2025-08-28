cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build types" FORCE)
endif()

project(Crimson
    LANGUAGES C CXX
    VERSION 1.0.0
    DESCRIPTION "Crimson Game Engine"
)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config/cmake")
include(detect_environment)

if(EMSCRIPTEN)
    set(WASM ON)
else()
    set(WASM OFF)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WASM)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    if(CMAKE_CONFIGURATION_TYPES)
        foreach(config ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${config} config_upper)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib/${config})
        endforeach()
    else()
        message(FATAL_ERROR "No valid multi-configuration build generators found. Please install one.")
    endif()
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
    set(SDL_X11 OFF CACHE BOOL "Disable X11 support" FORCE)
    set(SDL_WAYLAND OFF CACHE BOOL "Disable Wayland support" FORCE)

    set(BUILD_SHARED_LIBS OFF)
else()
    set(SDL_SHARED ON CACHE BOOL "Build SDL3 as shared library")
    set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as static library")

    set(BUILD_SHARED_LIBS ON)
endif()

set(SDL_TESTS OFF CACHE BOOL "Disable SDL3 tests")
set(SDL_EXAMPLES OFF CACHE BOOL "Disable SDL3 examples")

set(SDLIMAGE_TESTS OFF CACHE BOOL "Disable building SDL3_image tests")
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "Disable building SDL3_image samples")

set(SDLIMAGE_VENDORED ON CACHE BOOL "Use vendored third-party libraries")
set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable AVIF support")
set(SDLIMAGE_TIF OFF CACHE BOOL "Disable TIF support")
set(SDLIMAGE_WEBP OFF CACHE BOOL "Disable WEBP support")
set(SDLIMAGE_JXL OFF CACHE BOOL "Disable JXL support")
set(SDLIMAGE_PNG ON CACHE BOOL "Enable PNG support")
set(SDLIMAGE_JPG ON CACHE BOOL "Enable JPG support")
set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "Use stb_image for JPEG/PNG loading")

set(SDLMIXER_VENDORED ON CACHE BOOL "Use vendored SDL3_mixer libraries")
set(SDL3MIXER_ENABLE_MP3_DRMP3 ON CACHE BOOL "Enable MP3 support via dr_mp3")

if(WASM)
    set(SDLIMAGE_BMP OFF CACHE BOOL "Disable BMP support" FORCE)
    set(SDLIMAGE_GIF OFF CACHE BOOL "Disable GIF support" FORCE)

    set(SDL3MIXER_ENABLE_MP3_MPG123 OFF CACHE BOOL "Disable MP3 via mpg123 for WASM" FORCE)
    set(SDL3MIXER_ENABLE_OGG OFF CACHE BOOL "Disable OGG support for WASM" FORCE)
    set(SDL3MIXER_ENABLE_FLAC OFF CACHE BOOL "Disable FLAC support for WASM" FORCE)
    set(SDL3MIXER_MIDI OFF CACHE BOOL "Disable MIDI for WASM" FORCE)
    set(SDL3MIXER_OPUS OFF CACHE BOOL "Disable Opus support for WASM" FORCE)

    set(MPG123_SOURCES "")
    set_property(GLOBAL PROPERTY SDL_MIXER_EXCLUDE_MPG123 ON)
else()
    set(SDLIMAGE_BMP ON CACHE BOOL "Enable BMP support")
    set(SDLIMAGE_GIF ON CACHE BOOL "Enable GIF support")
endif()

if(NOT WASM)
    set(wxBUILD_SHARED OFF CACHE BOOL "Build wxWidgets as shared library" FORCE)
    set(wxBUILD_SAMPLES OFF CACHE BOOL "Disable wxWidgets samples" FORCE)
    set(wxBUILD_DEMOS OFF CACHE BOOL "Disable wxWidgets demos" FORCE)
    set(wxBUILD_TESTS OFF CACHE BOOL "Disable wxWidgets tests" FORCE)

    add_subdirectory(vendor/wxwidgets)
endif()

add_subdirectory(vendor/sdl3)
add_subdirectory(vendor/sdl3-image)
add_subdirectory(vendor/sdl3-mixer)

add_subdirectory(engine)

file(GLOB all_dirs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/games" "${CMAKE_CURRENT_SOURCE_DIR}/games/*")
set(AVAILABLE_GAMES "")

foreach(dir ${all_dirs})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/games/${dir}")
        add_subdirectory("games/${dir}")
        list(APPEND AVAILABLE_GAMES ${dir})
    endif()
endforeach()

message(STATUS "Available game targets: ${AVAILABLE_GAMES}")
add_subdirectory(launcher)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT launcher)
include(configure_vscode)
