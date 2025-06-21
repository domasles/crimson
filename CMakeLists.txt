cmake_minimum_required(VERSION 3.16)

project(Crimson
    LANGUAGES C CXX
    VERSION 1.0.0
    DESCRIPTION "Crimson Game Engine"
)

# ===== GLOBAL SETTINGS =====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# ===== OUTPUT DIRECTORIES =====
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Multi-config generators (Visual Studio, Xcode)
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib/${config})
endforeach()

# ===== PLATFORM DEFINITIONS =====
if(WIN32)
    add_compile_definitions(
        ENGINE_PLATFORM_WINDOWS
        GAME_PLATFORM_WINDOWS
        LAUNCHER_PLATFORM_WINDOWS
    )
elseif(UNIX AND NOT APPLE)
    add_compile_definitions(
        ENGINE_PLATFORM_LINUX
        GAME_PLATFORM_LINUX
        LAUNCHER_PLATFORM_LINUX
    )
endif()

# ===== BUILD CONFIGURATION DEFINITIONS =====
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(
        ENGINE_DEBUG
        GAME_DEBUG
        LAUNCHER_DEBUG
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(
        ENGINE_RELEASE
        GAME_RELEASE
        LAUNCHER_RELEASE
    )
endif()

# ===== COMPILER SETTINGS =====
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/utf-8 /MP)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fPIC)
    set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/../..")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

# ===== FUNCTION TO GENERATE VSCODE CONFIG =====
function(generate_vscode_config)
    # Create .vscode directory
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/.vscode")
      # Detect platform for configuration
    if(WIN32)
        set(PROBLEM_MATCHER "$msvc")
        set(DEBUGGER_TYPE "cppvsdbg")
        set(EXECUTABLE_PATH "\${workspaceFolder}/build/bin/Debug/launcher.exe")
        set(WORKING_DIR "\${workspaceFolder}/build/bin/Debug")
        set(GENERATOR "Visual Studio 17 2022")
        set(PREFERRED_GENERATORS "Visual Studio 17 2022" "Visual Studio 16 2019" "Ninja")
    else()
        set(PROBLEM_MATCHER "$gcc")
        set(DEBUGGER_TYPE "cppdbg")
        set(EXECUTABLE_PATH "\${workspaceFolder}/build/bin/launcher")
        set(WORKING_DIR "\${workspaceFolder}/build/bin")
        set(GENERATOR "Unix Makefiles")
        set(PREFERRED_GENERATORS "Unix Makefiles" "Ninja")
    endif()
    
    # Generate tasks.json
    file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/tasks.json" "{
    \"version\": \"2.0.0\",
    \"tasks\": [
        {
            \"label\": \"CMake: Build Debug\",
            \"type\": \"shell\",
            \"command\": \"cmake\",
            \"args\": [\"--build\", \"build\", \"--config\", \"Debug\"],
            \"group\": {
                \"kind\": \"build\",
                \"isDefault\": true
            },
            \"problemMatcher\": [\"${PROBLEM_MATCHER}\"],
            \"detail\": \"Build project in Debug mode\"
        },
        {
            \"label\": \"CMake: Build Release\",
            \"type\": \"shell\",
            \"command\": \"cmake\",
            \"args\": [\"--build\", \"build\", \"--config\", \"Release\"],
            \"group\": \"build\",
            \"problemMatcher\": [\"${PROBLEM_MATCHER}\"],
            \"detail\": \"Build project in Release mode\"
        },
        {
            \"label\": \"CMake: Clean\",
            \"type\": \"shell\",
            \"command\": \"cmake\",
            \"args\": [\"--build\", \"build\", \"--target\", \"clean\"],
            \"group\": \"build\",
            \"problemMatcher\": [\"${PROBLEM_MATCHER}\"],
            \"detail\": \"Clean build artifacts\"
        },
        {
            \"label\": \"CMake: Regenerate\",
            \"type\": \"shell\",
            \"command\": \"cmake\",
            \"args\": [\"-B\", \"build\", \"-S\", \".\"],
            \"group\": \"build\",
            \"problemMatcher\": [\"${PROBLEM_MATCHER}\"],
            \"detail\": \"Regenerate CMake build files\"
        }
    ]
}")

    # Generate launch.json
    file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/launch.json" "{
    \"version\": \"0.2.0\",
    \"configurations\": [
        {
            \"name\": \"Launch Crimson (Debug)\",
            \"type\": \"${DEBUGGER_TYPE}\",
            \"request\": \"launch\",
            \"program\": \"${EXECUTABLE_PATH}\",
            \"args\": [],
            \"stopAtEntry\": false,
            \"cwd\": \"${WORKING_DIR}\",
            \"environment\": [],
            \"console\": \"integratedTerminal\",
            \"preLaunchTask\": \"CMake: Build Debug\"
        }
    ]
}")

    # Generate settings.json
    string(REPLACE ";" "\",\n        \"" GENERATORS_JSON "${PREFERRED_GENERATORS}")
    file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/settings.json" "{
    \"cmake.buildDirectory\": \"\${workspaceFolder}/build\",
    \"cmake.configureOnOpen\": false,
    \"cmake.generator\": \"${GENERATOR}\",
    \"cmake.preferredGenerators\": [
        \"${GENERATORS_JSON}\"
    ],
    \"files.associations\": {
        \"*.h\": \"c\",
        \"*.hpp\": \"cpp\",
        \"*.cpp\": \"cpp\"
    },
    \"C_Cpp.default.configurationProvider\": \"ms-vscode.cmake-tools\",
    \"C_Cpp.default.compileCommands\": \"\${workspaceFolder}/build/compile_commands.json\",
    \"C_Cpp.intelliSenseEngine\": \"default\"
}")

    # Generate c_cpp_properties.json with dynamic compiler detection
    if(WIN32)
        set(COMPILER_PATH "")
        if(MSVC)
            # Try to get the actual MSVC path
            get_filename_component(COMPILER_PATH "${CMAKE_CXX_COMPILER}" ABSOLUTE)
        endif()
        if(NOT COMPILER_PATH OR NOT EXISTS "${COMPILER_PATH}")
            set(COMPILER_PATH "cl.exe")
        endif()
        
        file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json" "{
    \"version\": 4,
    \"configurations\": [
        {
            \"name\": \"Win32\",
            \"includePath\": [
                \"\${workspaceFolder}/**\",
                \"\${workspaceFolder}/engine/src/include/**\",
                \"\${workspaceFolder}/launcher/src/include/**\",
                \"\${workspaceFolder}/vendor/sdl3/include/**\",
                \"\${workspaceFolder}/vendor/sdl3-image/include/**\",
                \"\${workspaceFolder}/vendor/json/include/**\"
            ],
            \"defines\": [
                \"_DEBUG\",
                \"UNICODE\",
                \"_UNICODE\",
                \"WIN32\",
                \"_WIN32\",
                \"ENGINE_PLATFORM_WINDOWS\",
                \"GAME_PLATFORM_WINDOWS\",
                \"LAUNCHER_PLATFORM_WINDOWS\"
            ],
            \"compilerPath\": \"${COMPILER_PATH}\",
            \"cStandard\": \"c17\",
            \"cppStandard\": \"c++20\",
            \"intelliSenseMode\": \"windows-msvc-x64\",
            \"configurationProvider\": \"ms-vscode.cmake-tools\"
        }
    ]
}")
    else()
        file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json" "{
    \"version\": 4,
    \"configurations\": [
        {
            \"name\": \"Linux\",
            \"includePath\": [
                \"\${workspaceFolder}/**\",
                \"\${workspaceFolder}/engine/src/include/**\",
                \"\${workspaceFolder}/launcher/src/include/**\",
                \"\${workspaceFolder}/vendor/sdl3/include/**\",
                \"\${workspaceFolder}/vendor/sdl3-image/include/**\",
                \"\${workspaceFolder}/vendor/json/include/**\"
            ],
            \"defines\": [
                \"_DEBUG\",
                \"ENGINE_PLATFORM_LINUX\",
                \"GAME_PLATFORM_LINUX\",
                \"LAUNCHER_PLATFORM_LINUX\"
            ],
            \"compilerPath\": \"/usr/bin/gcc\",
            \"cStandard\": \"c17\",
            \"cppStandard\": \"c++17\",
            \"intelliSenseMode\": \"linux-gcc-x64\",
            \"configurationProvider\": \"ms-vscode.cmake-tools\"
        }
    ]
}")
    endif()

    message(STATUS "Generated VSCode configuration files in .vscode/")
endfunction()

# ===== SDL3 CONFIGURATION =====
set(SDL_SHARED ON CACHE BOOL "Build SDL3 as shared library")
set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as static library")
set(SDL_TESTS OFF CACHE BOOL "Build SDL3 tests")
set(SDL_EXAMPLES OFF CACHE BOOL "Build SDL3 examples")

# ===== SDL3_IMAGE CONFIGURATION =====
set(SDLIMAGE_VENDORED OFF CACHE BOOL "Use vendored third-party libraries")
set(SDLIMAGE_TESTS OFF CACHE BOOL "Build SDL3_image tests")
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "Build SDL3_image samples")
set(SDLIMAGE_AVIF OFF CACHE BOOL "Support loading AVIF images")
set(SDLIMAGE_TIF OFF CACHE BOOL "Support loading TIF images")
set(SDLIMAGE_WEBP OFF CACHE BOOL "Support loading WEBP images")
set(SDLIMAGE_JXL OFF CACHE BOOL "Support loading JXL images")
set(SDLIMAGE_PNG ON CACHE BOOL "Support loading PNG images")
set(SDLIMAGE_JPG ON CACHE BOOL "Support loading JPG images")
set(SDLIMAGE_BMP ON CACHE BOOL "Support loading BMP images")
set(SDLIMAGE_GIF ON CACHE BOOL "Support loading GIF images")
set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "Use stb_image for loading JPEG and PNG files")

# SDL3_image should be shared too when SDL3 is shared
set(BUILD_SHARED_LIBS ON)

# ===== DEPENDENCIES =====
add_subdirectory(vendor/sdl3)
add_subdirectory(vendor/sdl3-image)

# ===== PROJECT MODULES =====
add_subdirectory(engine)
add_subdirectory(games/crimson)
add_subdirectory(launcher)

# ===== IDE CONFIGURATION =====
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT launcher)

# Generate VSCode config files
generate_vscode_config()
