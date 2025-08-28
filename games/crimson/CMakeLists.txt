cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build types" FORCE)
endif()

project(crimson LANGUAGES CXX)

file(GLOB_RECURSE CRIMSON_SOURCES
    src/*.cpp
    src/*.h
)

if(WASM)
    set(libName "crimson_lib")
    add_library(${libName} STATIC ${CRIMSON_SOURCES})
else()
    set(libName "crimson")
    add_library(${libName} SHARED ${CRIMSON_SOURCES})
endif()

set_target_properties(${libName} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

if(WASM)
    set_target_properties(${libName} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/games/crimson"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/games/crimson"
    )
else()
    set_target_properties(${libName} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/crimson"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/games/crimson"
    )

    if(CMAKE_CONFIGURATION_TYPES)
        foreach(config ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${config} config_upper)

            set_target_properties(${libName} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/crimson"
                LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}/games/crimson"
            )
        endforeach()
    else()
        message(FATAL_ERROR "No valid multi-configuration build generators found. Please install one.")
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
