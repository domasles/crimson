function(configure_vscode_workspace)
    # Only configure if we're the top-level project
    if(NOT PROJECT_IS_TOP_LEVEL)
        return()
    endif()
    
    message(STATUS "Configuring VS Code workspace...")
    
    # Determine debugger type based on platform and compiler
    if(WIN32)
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            set(DEBUGGER_TYPE "cppvsdbg")
            set(INTELLISENSE_MODE "windows-msvc-x64")
        else()
            set(DEBUGGER_TYPE "cppdbg")
            set(INTELLISENSE_MODE "windows-clang-x64")
        endif()
    elseif(APPLE)
        set(DEBUGGER_TYPE "cppdbg")
        set(INTELLISENSE_MODE "macos-clang-x64")
    else()
        set(DEBUGGER_TYPE "cppdbg")
        set(INTELLISENSE_MODE "linux-gcc-x64")
    endif()
    
    # Set up paths - use relative paths where possible
    get_filename_component(SOURCE_DIR "${CMAKE_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(BUILD_DIR "${CMAKE_BINARY_DIR}" ABSOLUTE)
    set(WORKING_DIR "${SOURCE_DIR}")
    
    # Set executable paths - use the same structure as the original system
    if(WIN32)
        set(EXECUTABLE_SUFFIX ".exe")
    else()
        set(EXECUTABLE_SUFFIX "")
    endif()
    
    # Use the original executable naming and paths for compatibility
    set(EXECUTABLE_PATH "${BUILD_DIR}/bin/Debug/launcher${EXECUTABLE_SUFFIX}")
    set(EXECUTABLE_PATH_RELEASE "${BUILD_DIR}/bin/Release/launcher${EXECUTABLE_SUFFIX}")
    
    # Working directory should be where the executable is
    if(WIN32)
        set(WORKING_DIR "${BUILD_DIR}/bin/Debug")
    else()
        set(WORKING_DIR "${BUILD_DIR}/bin")
    endif()
    
    # Set compiler path
    if(CMAKE_CXX_COMPILER)
        set(COMPILER_PATH "${CMAKE_CXX_COMPILER}")
    else()
        set(COMPILER_PATH "")
    endif()
    
    # Set generator info
    if(CMAKE_GENERATOR)
        set(GENERATOR "${CMAKE_GENERATOR}")
    else()
        set(GENERATOR "")
    endif()
    
    # Set preferred generators as JSON array
    if(WIN32)
        set(PREFERRED_GENERATORS_JSON "[\"Visual Studio 17 2022\", \"Visual Studio 16 2019\", \"Ninja\"]")
    else()
        set(PREFERRED_GENERATORS_JSON "[\"Ninja\", \"Unix Makefiles\"]")
    endif()
    
    # Create .vscode directory if it doesn't exist
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/.vscode")
    
    # Configure templates if they exist
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
        configure_file(
            "${CONFIG_DIR}/c_cpp_properties.json.in"
            "${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json"
            @ONLY
        )
        message(STATUS "Configured .vscode/c_cpp_properties.json")
    endif()
    
endfunction()

# Auto-configure if VS Code is detected
if(CMAKE_EXPORT_COMPILE_COMMANDS OR DEFINED ENV{VSCODE_PID})
    configure_vscode_workspace()
endif()
