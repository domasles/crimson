#ifdef LAUNCHER_PLATFORM_WINDOWS
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <libgen.h>
    #include <dlfcn.h>
#endif

#include <iostream>
#include <format>
#include <string>

#include <functional>
