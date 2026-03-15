cmake_minimum_required(VERSION 3.24)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build types" FORCE)
endif()

project(Crimson
    LANGUAGES C CXX
    VERSION 1.2.0
    DESCRIPTION "Crimson Game Engine"
)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config/cmake")
include(detect_environment)

if(EMSCRIPTEN)
    set(WASM ON)
else()
    set(WASM OFF)
endif()

set(CMAKE_CXX_STANDARD 23)
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

set(SDLIMAGE_VENDORED OFF CACHE BOOL "Don't use vendored third-party libraries")
set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable AVIF support")
set(SDLIMAGE_WEBP OFF CACHE BOOL "Disable WEBP support")
set(SDLIMAGE_JXL OFF CACHE BOOL "Disable JXL support")
set(SDLIMAGE_GIF OFF CACHE BOOL "Disable GIF support")
set(SDLIMAGE_TIF OFF CACHE BOOL "Disable TIF support")

set(SDLIMAGE_BMP ON CACHE BOOL "Enable BMP support")
set(SDLIMAGE_PNG ON CACHE BOOL "Enable PNG support")
set(SDLIMAGE_JPG ON CACHE BOOL "Enable JPG support")

set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "Use stb_image for JPEG/PNG loading")

set(SDLIMAGE_PNG_VENDORED OFF CACHE BOOL "Don't use vendored libpng (STB handles PNG)")
set(SDLIMAGE_PNG_LIBPNG OFF CACHE BOOL "Don't use libpng (STB handles PNG)")

set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip zlib install" FORCE)
set(SKIP_INSTALL_FILES ON CACHE BOOL "Skip zlib file install" FORCE)

set(SDLMIXER_VENDORED OFF CACHE BOOL "Don't use vendored SDL3_mixer libraries")

set(SDLMIXER_WAVE ON CACHE BOOL "Enable WAV support")
set(SDLMIXER_AIFF OFF CACHE BOOL "Disable AIFF support")
set(SDLMIXER_VOC OFF CACHE BOOL "Disable VOC support")
set(SDLMIXER_AU OFF CACHE BOOL "Disable AU support")

set(SDLMIXER_MP3 ON CACHE BOOL "Enable MP3 support")
set(SDLMIXER_MP3_DRMP3 ON CACHE BOOL "Enable MP3 via dr_mp3 (header-only)")
set(SDLMIXER_MP3_MPG123 OFF CACHE BOOL "Disable mpg123 (redundant with dr_mp3)")

set(SDLMIXER_VORBIS_STB ON CACHE BOOL "Enable OGG Vorbis via stb_vorbis")
set(SDLMIXER_VORBIS_VORBISFILE OFF CACHE BOOL "Disable libvorbisfile (use stb instead)")
set(SDLMIXER_VORBIS_TREMOR OFF CACHE BOOL "Disable tremor")

set(SDLMIXER_FLAC OFF CACHE BOOL "Disable FLAC (lossless, huge files)")
set(SDLMIXER_OPUS OFF CACHE BOOL "Disable Opus (VoIP codec)")
set(SDLMIXER_WAVPACK OFF CACHE BOOL "Disable WavPack (lossless)")

set(SDLMIXER_GME OFF CACHE BOOL "Disable GME (retro console music)")
set(SDLMIXER_MOD OFF CACHE BOOL "Disable MOD (tracker music)")

set(SDLMIXER_MIDI OFF CACHE BOOL "Disable MIDI support")

add_subdirectory(vendor/sdl3)
add_subdirectory(vendor/sdl3-image)
add_subdirectory(vendor/sdl3-mixer)

if(NOT WASM)
    add_subdirectory(vendor/nuklear)
    add_subdirectory(vendor/glad)
endif()

set(_saved_build_shared ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/freetype)

if(NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()

set(FREETYPE_FOUND TRUE CACHE BOOL "" FORCE)

set(RMLUI_ENABLE_DEBUGGER OFF CACHE BOOL "" FORCE)
set(RMLUI_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(RMLUI_FONT_ENGINE "freetype" CACHE STRING "" FORCE)

add_subdirectory(vendor/rmlui)

set(BUILD_SHARED_LIBS ${_saved_build_shared} CACHE BOOL "" FORCE)

function(strip_debug_symbols target_name)
    if(MSVC)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND powershell -Command "if ('$<CONFIG>' -eq 'Release') { Write-Host 'Cleaning PDB files...'; Get-ChildItem -Path '${CMAKE_BINARY_DIR}/bin/Release' -Filter '*.pdb' -Recurse -ErrorAction SilentlyContinue | Remove-Item -Force -ErrorAction SilentlyContinue }"
            VERBATIM
        )
    endif()
endfunction()

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
