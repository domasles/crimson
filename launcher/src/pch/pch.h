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

#include <iostream>
#include <format>
#include <string>

#include <functional>
