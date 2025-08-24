cmake_minimum_required(VERSION 3.16)
project(outBreak LANGUAGES CXX)

file(GLOB_RECURSE OUTBREAK_SOURCES
    src/*.cpp
    src/*.h
)

if(WASM)
    add_library(outBreak STATIC ${OUTBREAK_SOURCES})
else()
    add_library(outBreak SHARED ${OUTBREAK_SOURCES})
endif()

set_target_properties(outBreak PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

if(WASM)
    set_target_properties(outBreak PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/games/outBreak"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/games/outBreak"
    )
else()
    set_target_properties(outBreak PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/outBreak"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/outBreak"
    )

    foreach(config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${config} config_upper)

        set_target_properties(outBreak PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/outBreak"
            LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/outBreak"
        )
    endforeach()
endif()

target_include_directories(outBreak
    PRIVATE
        src/pch
        src/include
        ../../engine/src/include
        ../../vendor/json/include
        ../../vendor/sdl3/include
        ../../vendor/sdl3-image/include
        ../../vendor/sdl3-mixer/include
)

target_link_libraries(outBreak
    PRIVATE
        engine
)

if(WASM)
    target_compile_definitions(outBreak PRIVATE GAME_PLATFORM_EMSCRIPTEN)
elseif(WIN32)
    target_compile_definitions(outBreak PRIVATE GAME_PLATFORM_WINDOWS)
    set_property(TARGET outBreak PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(APPLE)
    target_compile_definitions(outBreak PRIVATE GAME_PLATFORM_MACOS)

    target_link_options(outBreak PRIVATE
        -Wl,-rpath,@executable_path
        -Wl,-rpath,@executable_path/../..
    )
elseif(UNIX)
    target_compile_definitions(outBreak PRIVATE GAME_PLATFORM_LINUX)

    target_link_options(outBreak PRIVATE
        -Wl,-rpath,'$ORIGIN'
        -Wl,-rpath-link,'$ORIGIN'
        -Wl,-rpath,'$ORIGIN/../../'
        -Wl,-rpath-link,'$ORIGIN/../../'
    )
endif()

target_compile_definitions(outBreak PRIVATE
    $<$<CONFIG:Debug>:GAME_DEBUG>
    $<$<CONFIG:Release>:GAME_RELEASE>
)

target_precompile_headers(outBreak PRIVATE src/pch/pch.h)

add_custom_command(TARGET outBreak POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/config"
    "$<TARGET_FILE_DIR:outBreak>/config"
    COMMENT "Copying config directory to output"
)

add_custom_command(TARGET outBreak POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:outBreak>/assets"
    COMMENT "Copying assets directory to output"
)
