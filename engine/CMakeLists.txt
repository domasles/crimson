cmake_minimum_required(VERSION 3.24)

project(Engine LANGUAGES CXX)

file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

add_library(engine STATIC ${ENGINE_SOURCES})

target_include_directories(engine
    PUBLIC
        src/include
        ${CMAKE_SOURCE_DIR}/vendor/json/include
    PRIVATE
        src/pch
)

target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/rmlui/Include)

if(WIN32)
    target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)
endif()

if(WASM)
    target_link_libraries(engine
        PUBLIC
            SDL3::SDL3-static
            SDL3_image::SDL3_image-static
            SDL3_mixer::SDL3_mixer-static
            RmlUi::Core
    )
else()
    target_link_libraries(engine
        PUBLIC
            SDL3::SDL3-shared
            SDL3_image::SDL3_image-shared
            SDL3_mixer::SDL3_mixer-shared
        PRIVATE
            glad
            RmlUi::Core
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
