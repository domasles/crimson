cmake_minimum_required(VERSION 3.16)
project(Launcher LANGUAGES CXX)

# ===== SOURCE FILES =====
file(GLOB_RECURSE LAUNCHER_SOURCES
    src/*.cpp
    src/*.h
)

# ===== CREATE EXECUTABLE =====
add_executable(launcher ${LAUNCHER_SOURCES})

# ===== PROPERTIES =====
set_target_properties(launcher PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# ===== INCLUDE DIRECTORIES =====
target_include_directories(launcher
    PRIVATE
        src/include
        src/pch
        ../engine/src/include
)

# ===== LINK LIBRARIES =====
target_link_libraries(launcher
    PRIVATE
        crimson
)

# ===== PLATFORM SPECIFIC =====
if(WIN32)
    target_compile_definitions(launcher PRIVATE LAUNCHER_PLATFORM_WINDOWS)
    set_property(TARGET launcher PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(launcher PRIVATE LAUNCHER_PLATFORM_LINUX)
endif()

# ===== BUILD CONFIGURATION =====
target_compile_definitions(launcher PRIVATE
    $<$<CONFIG:Debug>:LAUNCHER_DEBUG>
    $<$<CONFIG:Release>:LAUNCHER_RELEASE>
)

# ===== PRECOMPILED HEADERS =====
target_precompile_headers(launcher PRIVATE src/pch/pch.h)

# ===== DEPENDENCIES =====
add_dependencies(launcher crimson)

# ===== POST BUILD COMMANDS =====
# Copy SDL3 DLLs to launcher output directory
if(WIN32)
    add_custom_command(TARGET launcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3-shared>"
        "$<TARGET_FILE_DIR:launcher>"
        COMMENT "Copying SDL3.dll to launcher output directory"
    )
    
    add_custom_command(TARGET launcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_image::SDL3_image-shared>"
        "$<TARGET_FILE_DIR:launcher>"
        COMMENT "Copying SDL3_image.dll to launcher output directory"
    )
endif()
