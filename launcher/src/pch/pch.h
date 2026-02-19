#pragma once

#ifdef LAUNCHER_PLATFORM_WINDOWS
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#elif defined(LAUNCHER_PLATFORM_MACOS)
    #include <TargetConditionals.h>
    #include <unistd.h>
    #include <libgen.h>
    #include <dlfcn.h>
    #include <mach-o/dyld.h>
#else
    #include <unistd.h>
    #include <libgen.h>
    #include <dlfcn.h>
#endif

#ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
    #include <SDL3/SDL.h>

    #define NK_INCLUDE_STANDARD_VARARGS
    #define NK_INCLUDE_STANDARD_IO
    #define NK_INCLUDE_FONT_BAKING
    #define NK_INCLUDE_DEFAULT_FONT
    #define NK_INCLUDE_COMMAND_USERDATA
    #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT

    #include <nuklear.h>

    extern "C" {
        #include <nuklear_sdl3_renderer.h>
    }
#endif

#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>

#include <utility>
#include <format>
#include <string>
#include <vector>

#include <functional>
#include <optional>
