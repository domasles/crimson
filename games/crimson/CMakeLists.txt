cmake_minimum_required(VERSION 3.16)
project(Crimson LANGUAGES CXX)

# ===== SOURCE FILES =====
set(CRIMSON_SOURCES
    src/main.cpp
    src/sceneRegistrator.cpp
)

# Get all files from entities and scenes directories
file(GLOB_RECURSE CRIMSON_ENTITY_FILES CONFIGURE_DEPENDS
    src/entities/*.cpp
    src/entities/*.h
    src/entities/*.hpp
)

file(GLOB_RECURSE CRIMSON_SCENE_FILES CONFIGURE_DEPENDS
    src/scenes/*.cpp
    src/scenes/*.h
    src/scenes/*.hpp
)

file(GLOB_RECURSE CRIMSON_INCLUDE_FILES CONFIGURE_DEPENDS
    src/include/*.h
    src/include/*.hpp
)

set(CRIMSON_PCH_FILES
    src/pch/pch.cpp
    src/pch/pch.h
)

# Combine all sources
list(APPEND CRIMSON_SOURCES ${CRIMSON_ENTITY_FILES} ${CRIMSON_SCENE_FILES} ${CRIMSON_INCLUDE_FILES} ${CRIMSON_PCH_FILES})

# ===== CREATE LIBRARY =====
add_library(crimson SHARED ${CRIMSON_SOURCES})

# ===== PROPERTIES =====
set_target_properties(crimson PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/crimson"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/crimson"
)

# Multi-config generators
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config_upper)
    set_target_properties(crimson PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/crimson"
        LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/crimson"
    )
endforeach()

# ===== INCLUDE DIRECTORIES =====
target_include_directories(crimson
    PRIVATE
        src/pch
        src/include
        ../../engine/src/include        ../../vendor/json/include
        ../../vendor/sdl3/include
        ../../vendor/sdl3-image/include
)

# ===== LINK LIBRARIES =====
target_link_libraries(crimson
    PRIVATE
        engine
)

# ===== PLATFORM SPECIFIC =====
if(WIN32)
    target_compile_definitions(crimson PRIVATE GAME_PLATFORM_WINDOWS)
    set_property(TARGET crimson PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(crimson PRIVATE GAME_PLATFORM_LINUX)
    target_link_options(crimson PRIVATE
        -Wl,-rpath,'$ORIGIN'
        -Wl,-rpath-link,'$ORIGIN'
        -Wl,-rpath,'$ORIGIN/../../'
        -Wl,-rpath-link,'$ORIGIN/../../'
    )
endif()

# ===== BUILD CONFIGURATION =====
target_compile_definitions(crimson PRIVATE
    $<$<CONFIG:Debug>:GAME_DEBUG>
    $<$<CONFIG:Release>:GAME_RELEASE>
)

# ===== PRECOMPILED HEADERS =====
target_precompile_headers(crimson PRIVATE src/pch/pch.h)

# ===== POST BUILD COMMANDS =====
# Copy config and assets directories
add_custom_command(TARGET crimson POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/config"
    "$<TARGET_FILE_DIR:crimson>/config"
    COMMENT "Copying config directory to output"
)

add_custom_command(TARGET crimson POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:crimson>/assets"
    COMMENT "Copying assets directory to output"
)
