cmake_minimum_required(VERSION 3.16)
project(Engine LANGUAGES CXX)

file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/*.h
)

add_library(engine STATIC ${ENGINE_SOURCES})

set_target_properties(engine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(engine
    PUBLIC
        src/include
        ${CMAKE_SOURCE_DIR}/vendor/json/include
    PRIVATE
        src/pch
)

if(WASM)
    target_link_libraries(engine
        PUBLIC
            SDL3::SDL3-static
            SDL3_image::SDL3_image-static
            SDL3_mixer::SDL3_mixer-static
    )
else()
    target_link_libraries(engine
        PUBLIC
            SDL3::SDL3-shared
            SDL3_image::SDL3_image-shared
            SDL3_mixer::SDL3_mixer-shared
    )
endif()

if(WASM)
    target_compile_definitions(engine PRIVATE ENGINE_PLATFORM_EMSCRIPTEN)
elseif(WIN32)
    target_compile_definitions(engine PRIVATE ENGINE_PLATFORM_WINDOWS)
    set_property(TARGET engine PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(APPLE)
    target_compile_definitions(engine PRIVATE ENGINE_PLATFORM_MACOS)
    target_compile_options(engine PRIVATE -fPIC)
elseif(UNIX)
    target_compile_definitions(engine PRIVATE ENGINE_PLATFORM_LINUX)
    target_compile_options(engine PRIVATE -fPIC)
endif()

target_compile_definitions(engine PRIVATE
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
)

target_precompile_headers(engine PRIVATE src/pch/pch.h)
