cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build types" FORCE)
endif()

project(Launcher LANGUAGES CXX)

file(GLOB_RECURSE LAUNCHER_SOURCES
    src/*.cpp
    src/*.h
)

if(WASM)
    set(OUTPUT_NAME "index" CACHE STRING "Executable output name (default: index)")

    foreach(game ${AVAILABLE_GAMES})
        set(GAME_LIB "${game}_lib")
        set(GAME_EXEC "${game}")

        add_executable(${GAME_EXEC} ${LAUNCHER_SOURCES})

        if(EXISTS "${CMAKE_SOURCE_DIR}/games/${game}/assets")
            target_link_options(${GAME_EXEC} PRIVATE "--preload-file=${CMAKE_SOURCE_DIR}/games/${game}/assets@assets")
        endif()

        if(EXISTS "${CMAKE_SOURCE_DIR}/games/${game}/config")
            target_link_options(${GAME_EXEC} PRIVATE "--preload-file=${CMAKE_SOURCE_DIR}/games/${game}/config@config")
        endif()

        if(EXISTS "${CMAKE_SOURCE_DIR}/engine/shaders")
            target_link_options(${GAME_EXEC} PRIVATE "--preload-file=${CMAKE_SOURCE_DIR}/engine/shaders@shaders/engine")
        endif()

        target_link_options(${GAME_EXEC} PRIVATE
            "-sWASM=1"
            "-sUSE_SDL=3"
            "-sFULL_ES3=1"
            "-sMAX_WEBGL_VERSION=2"
            "-sALLOW_MEMORY_GROWTH=1"
            "-sNO_DISABLE_EXCEPTION_CATCHING=1"
            "--shell-file=${CMAKE_SOURCE_DIR}/config/wasm/index.html"
        )

        set_target_properties(${GAME_EXEC} PROPERTIES
            OUTPUT_NAME "${OUTPUT_NAME}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/${game}"
            SUFFIX ".html"
        )

        target_include_directories(${GAME_EXEC} PRIVATE
            src/include
            src/pch
            ../engine/src/include
            ../vendor/json/include
        )

        target_link_libraries(${GAME_EXEC} PRIVATE ${GAME_LIB})
        add_dependencies(${GAME_EXEC} ${GAME_LIB})

        target_compile_definitions(${GAME_EXEC} PRIVATE 
            LAUNCHER_PLATFORM_EMSCRIPTEN
            GAME_TARGET="${game}"
        )

        target_precompile_headers(${GAME_EXEC} PRIVATE src/pch/pch.h)
    endforeach()
else()
    add_executable(launcher ${LAUNCHER_SOURCES})
    set(LAUNCHER_TARGET launcher)

    target_link_libraries(${LAUNCHER_TARGET}
        PRIVATE
            nuklear
            SDL3::SDL3-shared
    )

    add_dependencies(${LAUNCHER_TARGET} ${AVAILABLE_GAMES})

    if(WIN32)
        set_target_properties(launcher PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>)
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(${LAUNCHER_TARGET} PRIVATE pthread dl)
    endif()

    target_include_directories(${LAUNCHER_TARGET}
        PRIVATE
            assets/fonts/inter
            src/include
            src/pch
            ../engine/src/include
            ../vendor/json/include
    )

    target_precompile_headers(${LAUNCHER_TARGET} PRIVATE src/pch/pch.h)

    strip_debug_symbols(${LAUNCHER_TARGET})

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
