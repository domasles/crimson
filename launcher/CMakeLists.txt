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
    foreach(game ${AVAILABLE_GAMES})
        set(GAME_LAUNCHER_TARGET "${game}_launcher")
        add_executable(${GAME_LAUNCHER_TARGET} ${LAUNCHER_SOURCES})
        set(PRELOAD_FLAGS_LIST "")

        if(EXISTS "${CMAKE_SOURCE_DIR}/games/${game}/assets")
            list(APPEND PRELOAD_FLAGS_LIST
                "--preload-file=${CMAKE_SOURCE_DIR}/games/${game}/assets@assets"
            )
        endif()

        if(EXISTS "${CMAKE_SOURCE_DIR}/games/${game}/config")
            list(APPEND PRELOAD_FLAGS_LIST
                "--preload-file=${CMAKE_SOURCE_DIR}/games/${game}/config@config"
            )
        endif()

        string(JOIN " " PRELOAD_FLAGS_STR ${PRELOAD_FLAGS_LIST})

        set_target_properties(${GAME_LAUNCHER_TARGET} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            OUTPUT_NAME "index"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/${game}"
            LINK_FLAGS "-s WASM=1 -s USE_SDL=2 ${PRELOAD_FLAGS_STR} -s ALLOW_MEMORY_GROWTH=1 -s NO_DISABLE_EXCEPTION_CATCHING=1 --shell-file ${CMAKE_SOURCE_DIR}/config/wasm/index.html"
            SUFFIX ".html"
        )

        target_include_directories(${GAME_LAUNCHER_TARGET}
            PRIVATE
                src/include
                src/pch
                ../engine/src/include
                ../vendor/json/include
        )

        target_link_libraries(${GAME_LAUNCHER_TARGET}
            PRIVATE
                ${game}
        )

        target_compile_definitions(${GAME_LAUNCHER_TARGET} PRIVATE 
            LAUNCHER_PLATFORM_EMSCRIPTEN
            GAME_TARGET="${game}"
        )

        target_precompile_headers(${GAME_LAUNCHER_TARGET} PRIVATE src/pch/pch.h)
        add_dependencies(${GAME_LAUNCHER_TARGET} ${game})
    endforeach()
else()
    add_executable(launcher ${LAUNCHER_SOURCES})
    set(LAUNCHER_TARGET launcher)

    set_target_properties(${LAUNCHER_TARGET} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(${LAUNCHER_TARGET}
        PRIVATE
            wx::core
            wx::base
    )

    if(WIN32)
        set_target_properties(launcher PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>)
    elseif(APPLE)
        set_target_properties(launcher PROPERTIES MACOSX_BUNDLE $<$<CONFIG:Release>:TRUE>)
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(${LAUNCHER_TARGET} PRIVATE
            X11
            Xext
            Xi
            Xrandr
            SM
            ICE
            pthread
            dl
        )
    endif()

    target_include_directories(${LAUNCHER_TARGET}
        PRIVATE
            src/include
            src/pch
            ../engine/src/include
            ../vendor/json/include
    )

    target_precompile_headers(${LAUNCHER_TARGET} PRIVATE src/pch/pch.h)

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
