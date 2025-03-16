#ifdef LAUNCHER_PLATFORM_WINDOWS
    #include <Windows.h>
#else
    #include <dlfcn.h>
    #include <unistd.h>
    #include <libgen.h>
#endif

#include <iostream>
#include <string>
