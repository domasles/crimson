cmake_minimum_required(VERSION 3.16)
project(Engine LANGUAGES CXX)

# ===== SOURCE FILES =====
set(ENGINE_SOURCES
    src/Core.cpp
    src/Input.cpp
    src/Map.cpp
    src/Scene.cpp
    src/Texture.cpp
    src/utils/filesystem.cpp
    src/utils/math.cpp
    src/utils/rendering.cpp
)

set(ENGINE_HEADERS
    src/include/Core.h
    src/include/Engine.h
    src/include/Entity.h
    src/include/Input.h
    src/include/Map.h
    src/include/Scene.h
    src/include/Texture.h
    src/include/utils/filesystem.h
    src/include/utils/math.h
    src/include/utils/rendering.h
    src/include/utils/tileset.h
)

set(ENGINE_PCH_FILES
    src/pch/pch.cpp
    src/pch/pch.h
)

# ===== CREATE LIBRARY =====
add_library(engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${ENGINE_PCH_FILES})

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
