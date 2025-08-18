function(detect_platform_settings)
    if(WIN32)
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            set(DEBUGGER_TYPE "cppvsdbg" PARENT_SCOPE)
            set(INTELLISENSE_MODE "windows-msvc-x64" PARENT_SCOPE)
        else()
            set(DEBUGGER_TYPE "cppdbg" PARENT_SCOPE)
            set(INTELLISENSE_MODE "windows-clang-x64" PARENT_SCOPE)
        endif()
        set(PREFERRED_GENERATORS_JSON "[\"Visual Studio 17 2022\", \"Visual Studio 16 2019\", \"Ninja\"]" PARENT_SCOPE)
    elseif(APPLE)
        set(DEBUGGER_TYPE "cppdbg" PARENT_SCOPE)
        set(INTELLISENSE_MODE "macos-clang-x64" PARENT_SCOPE)
        set(PREFERRED_GENERATORS_JSON "[\"Ninja\", \"Unix Makefiles\"]" PARENT_SCOPE)
    else()
        set(DEBUGGER_TYPE "cppdbg" PARENT_SCOPE)
        set(INTELLISENSE_MODE "linux-gcc-x64" PARENT_SCOPE)
        set(PREFERRED_GENERATORS_JSON "[\"Ninja\", \"Unix Makefiles\"]" PARENT_SCOPE)
    endif()
endfunction()

function(setup_paths_and_executables)
    get_filename_component(SOURCE_DIR "${CMAKE_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(BUILD_DIR "${CMAKE_BINARY_DIR}" ABSOLUTE)

    set(SOURCE_DIR "${SOURCE_DIR}" PARENT_SCOPE)
    set(BUILD_DIR "${BUILD_DIR}" PARENT_SCOPE)

    if(WIN32)
        set(EXECUTABLE_SUFFIX ".exe" PARENT_SCOPE)
    else()
        set(EXECUTABLE_SUFFIX "" PARENT_SCOPE)
    endif()

    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG)
        set(DEBUG_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
    else()
        set(DEBUG_OUTPUT_DIR "${BUILD_DIR}/bin/Debug")
    endif()
    
    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE)
        set(RELEASE_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")
    else()
        set(RELEASE_OUTPUT_DIR "${BUILD_DIR}/bin/Release")
    endif()

    set(WORKING_DIR "${DEBUG_OUTPUT_DIR}" PARENT_SCOPE)
    set(EXECUTABLE_PATH "${DEBUG_OUTPUT_DIR}/launcher${EXECUTABLE_SUFFIX}" PARENT_SCOPE)
    set(EXECUTABLE_PATH_RELEASE "${RELEASE_OUTPUT_DIR}/launcher${EXECUTABLE_SUFFIX}" PARENT_SCOPE)
endfunction()

function(collect_build_info)
    if(CMAKE_CXX_COMPILER)
        set(COMPILER_PATH "${CMAKE_CXX_COMPILER}" PARENT_SCOPE)
    else()
        set(COMPILER_PATH "" PARENT_SCOPE)
    endif()

    if(CMAKE_GENERATOR)
        set(GENERATOR "${CMAKE_GENERATOR}" PARENT_SCOPE)
    else()
        set(GENERATOR "" PARENT_SCOPE)
    endif()
endfunction()

function(generate_cpp_properties_defines)
    if(CMAKE_BUILD_TYPE)
        set(CONFIG_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}" PARENT_SCOPE)
    else()
        set(CONFIG_NAME "${CMAKE_SYSTEM_NAME}" PARENT_SCOPE)
    endif()

    get_directory_property(CMAKE_COMPILE_DEFS COMPILE_DEFINITIONS)
    set(DEFINES_LIST "")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND DEFINES_LIST "\"DEBUG\"")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        list(APPEND DEFINES_LIST "\"NDEBUG\"")
    endif()

    if(WIN32)
        list(APPEND DEFINES_LIST "\"UNICODE\"" "\"_UNICODE\"" "\"WIN32\"" "\"_WIN32\"" "\"_WINDOWS\"")
    elseif(APPLE)
        list(APPEND DEFINES_LIST "\"__APPLE__\"")
    elseif(UNIX)
        list(APPEND DEFINES_LIST "\"__linux__\"")
    endif()

    if(CMAKE_COMPILE_DEFS)
        foreach(def ${CMAKE_COMPILE_DEFS})
            list(APPEND DEFINES_LIST "\"${def}\"")
        endforeach()
    endif()

    if(DEFINES_LIST)
        string(REPLACE ";" ",\n        " DEFINES "${DEFINES_LIST}")
    else()
        set(DEFINES "")
    endif()

    set(DEFINES "${DEFINES}" PARENT_SCOPE)
endfunction()

function(configure_vscode_workspace)
    if(NOT PROJECT_IS_TOP_LEVEL)
        return()
    endif()

    message(STATUS "Configuring VS Code workspace...")

    detect_platform_settings()
    setup_paths_and_executables()
    collect_build_info()

    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/.vscode")
    set(CONFIG_DIR "${CMAKE_SOURCE_DIR}/config/vscode")

    if(EXISTS "${CONFIG_DIR}/launch.json.in")
        configure_file(
            "${CONFIG_DIR}/launch.json.in"
            "${CMAKE_SOURCE_DIR}/.vscode/launch.json"
            @ONLY
        )

        message(STATUS "Configured .vscode/launch.json")
    endif()

    if(EXISTS "${CONFIG_DIR}/tasks.json.in")
        configure_file(
            "${CONFIG_DIR}/tasks.json.in"
            "${CMAKE_SOURCE_DIR}/.vscode/tasks.json"
            @ONLY
        )

        message(STATUS "Configured .vscode/tasks.json")
    endif()

    if(EXISTS "${CONFIG_DIR}/settings.json.in")
        configure_file(
            "${CONFIG_DIR}/settings.json.in"
            "${CMAKE_SOURCE_DIR}/.vscode/settings.json"
            @ONLY
        )

        message(STATUS "Configured .vscode/settings.json")
    endif()

    if(EXISTS "${CONFIG_DIR}/c_cpp_properties.json.in")
        generate_cpp_properties_defines()

        configure_file(
            "${CONFIG_DIR}/c_cpp_properties.json.in"
            "${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json"
            @ONLY
        )

        message(STATUS "Configured .vscode/c_cpp_properties.json for ${CONFIG_NAME}")
    endif()

endfunction()

if(CMAKE_EXPORT_COMPILE_COMMANDS OR DEFINED ENV{VSCODE_PID})
    configure_vscode_workspace()
endif()
