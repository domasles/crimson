cmake_minimum_required(VERSION 3.16)
project(Engine LANGUAGES CXX)

# ===== SOURCE FILES =====
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/*.h
)

# ===== CREATE LIBRARY =====
add_library(engine STATIC ${ENGINE_SOURCES})

# ===== PROPERTIES =====
set_target_properties(engine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# ===== INCLUDE DIRECTORIES =====
target_include_directories(engine
    PUBLIC
        src/include
        ${CMAKE_SOURCE_DIR}/vendor/json/include
    PRIVATE
        src/pch
)

# ===== LINK LIBRARIES =====
target_link_libraries(engine
    PUBLIC
        SDL3::SDL3-shared
        SDL3_image::SDL3_image-shared
)

# ===== PLATFORM SPECIFIC =====
if(WIN32)
    target_compile_definitions(engine PRIVATE ENGINE_PLATFORM_WINDOWS)
    set_property(TARGET engine PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(engine PRIVATE ENGINE_PLATFORM_LINUX)
    target_compile_options(engine PRIVATE -fPIC)
endif()

# ===== BUILD CONFIGURATION =====
target_compile_definitions(engine PRIVATE
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
)

# ===== PRECOMPILED HEADERS =====
target_precompile_headers(engine PRIVATE src/pch/pch.h)
