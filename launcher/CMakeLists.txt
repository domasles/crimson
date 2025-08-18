cmake_minimum_required(VERSION 3.16)
project(Launcher LANGUAGES CXX)

file(GLOB_RECURSE LAUNCHER_SOURCES
    src/*.cpp
    src/*.h
)

if(WASM)
    add_executable(index ${LAUNCHER_SOURCES})
    set(LAUNCHER_TARGET index)
else()
    add_executable(launcher ${LAUNCHER_SOURCES})
    set(LAUNCHER_TARGET launcher)
endif()

set_target_properties(${LAUNCHER_TARGET} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(${LAUNCHER_TARGET}
    PRIVATE
        src/include
        src/pch
        ../engine/src/include
)

target_link_libraries(${LAUNCHER_TARGET}
    PRIVATE
        crimson
)

if(WASM)
    set_target_properties(${LAUNCHER_TARGET} PROPERTIES
        LINK_FLAGS "-s WASM=1 -s USE_SDL=2 --preload-file ${CMAKE_SOURCE_DIR}/games/crimson/assets@assets --preload-file ${CMAKE_SOURCE_DIR}/games/crimson/config@config -s ALLOW_MEMORY_GROWTH=1 -s NO_DISABLE_EXCEPTION_CATCHING=1 --shell-file ${CMAKE_SOURCE_DIR}/config/wasm/index.html"
        SUFFIX ".html"
    )
endif()

if(WASM)
    target_compile_definitions(${LAUNCHER_TARGET} PRIVATE LAUNCHER_PLATFORM_EMSCRIPTEN)
elseif(WIN32)
    target_compile_definitions(${LAUNCHER_TARGET} PRIVATE LAUNCHER_PLATFORM_WINDOWS)
    set_property(TARGET ${LAUNCHER_TARGET} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(APPLE)
    target_compile_definitions(${LAUNCHER_TARGET} PRIVATE LAUNCHER_PLATFORM_MACOS)
elseif(UNIX)
    target_compile_definitions(${LAUNCHER_TARGET} PRIVATE LAUNCHER_PLATFORM_LINUX)
endif()

target_compile_definitions(${LAUNCHER_TARGET} PRIVATE
    $<$<CONFIG:Debug>:LAUNCHER_DEBUG>
    $<$<CONFIG:Release>:LAUNCHER_RELEASE>
)

target_precompile_headers(${LAUNCHER_TARGET} PRIVATE src/pch/pch.h)
add_dependencies(${LAUNCHER_TARGET} crimson)

if(WIN32 AND NOT WASM)
    add_custom_command(TARGET ${LAUNCHER_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3-shared>"
        "$<TARGET_FILE_DIR:${LAUNCHER_TARGET}>"
        COMMENT "Copying SDL3 library to launcher output directory"
    )

    add_custom_command(TARGET ${LAUNCHER_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_image::SDL3_image-shared>"
        "$<TARGET_FILE_DIR:${LAUNCHER_TARGET}>"
        COMMENT "Copying SDL3_image library to launcher output directory"
    )

    add_custom_command(TARGET ${LAUNCHER_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_mixer::SDL3_mixer-shared>"
        "$<TARGET_FILE_DIR:${LAUNCHER_TARGET}>"
        COMMENT "Copying SDL3_mixer library to launcher output directory"
    )
endif()
