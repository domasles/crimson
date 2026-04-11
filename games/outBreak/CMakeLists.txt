cmake_minimum_required(VERSION 3.24)

project(outBreak LANGUAGES CXX)

file(GLOB_RECURSE OUTBREAK_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${OUTBREAK_SOURCES})

if(WASM)
    set(libName "outBreak_lib")
    add_library(${libName} STATIC ${OUTBREAK_SOURCES})
else()
    set(libName "outBreak")
    add_library(${libName} SHARED ${OUTBREAK_SOURCES})
endif()

if(WASM)
    set_target_properties(${libName} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/games/outBreak"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/games/outBreak"
    )
else()
    set_target_properties(${libName} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/outBreak"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/outBreak"
    )

    if(CMAKE_CONFIGURATION_TYPES)
        foreach(config ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${config} config_upper)

            set_target_properties(${libName} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/outBreak"
                LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/outBreak"
            )
        endforeach()
    else()
        message(FATAL_ERROR "No valid multi-configuration build generators found, please install one")
    endif()
endif()

target_include_directories(${libName}
    PRIVATE
        src/pch
        src/include
        ../../engine/src/include
        ../../vendor/json/include
        ../../vendor/sdl3/include
        ../../vendor/sdl3-image/include
        ../../vendor/sdl3-mixer/include
)

target_link_libraries(${libName}
    PRIVATE
        engine
)

if(WASM)
    target_compile_definitions(${libName} PRIVATE GAME_PLATFORM_EMSCRIPTEN)
elseif(WIN32)
    target_compile_definitions(${libName} PRIVATE GAME_PLATFORM_WINDOWS)
    set_property(TARGET ${libName} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(APPLE)
    target_compile_definitions(${libName} PRIVATE GAME_PLATFORM_MACOS)

    target_link_options(${libName} PRIVATE
        -Wl,-rpath,@executable_path
        -Wl,-rpath,@executable_path/../..
    )
elseif(UNIX)
    target_compile_definitions(${libName} PRIVATE GAME_PLATFORM_LINUX)

    target_link_options(${libName} PRIVATE
        -Wl,-rpath,'$ORIGIN'
        -Wl,-rpath-link,'$ORIGIN'
        -Wl,-rpath,'$ORIGIN/../../'
        -Wl,-rpath-link,'$ORIGIN/../../'
    )
endif()

target_compile_definitions(${libName} PRIVATE
    $<$<CONFIG:Debug>:GAME_DEBUG>
    $<$<CONFIG:Release>:GAME_RELEASE>
)

target_precompile_headers(${libName} PRIVATE src/pch/pch.h)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config")
    add_custom_command(TARGET ${libName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/config"
        "$<TARGET_FILE_DIR:${libName}>/config"
        COMMENT "Copying config directory to output"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${libName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${libName}>/assets"
        COMMENT "Copying assets directory to output"
    )
endif()

# Copy engine shaders
add_custom_command(TARGET ${libName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/engine/shaders"
    "$<TARGET_FILE_DIR:${libName}>/shaders/engine"
    COMMENT "Copying engine shaders to output"
)

# Copy game-specific shaders
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
    add_custom_command(TARGET ${libName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:${libName}>/shaders"
        COMMENT "Copying game-specific shaders to output"
    )
endif()
