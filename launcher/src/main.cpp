#include <pch.h>

#include <utils/filesystem.h>
#include <utils/libLoader.h>

#ifdef LAUNCHER_PLATFORM_WINDOWS
    typedef void (*initGameFunc)();
    typedef void (*updateGameFunc)();
#endif

using namespace launcher::utils::filesystem;
using namespace launcher::utils::libLoader;

int main() {
    #ifdef LAUNCHER_PLATFORM_WINDOWS
        const std::string fileName = "games/crimson/crimson.dll";
    #else
        const std::string fileName = "games/crimson/libcrimson.so";
    #endif

    void* libraryHandle = loadLibrary(fileName);

    if (!libraryHandle) return -1;

    auto init = reinterpret_cast<void(*)()>(getFunction(libraryHandle, "init"));
    auto update = reinterpret_cast<void(*)()>(getFunction(libraryHandle, "update"));

    init();
    update();

    unloadLibrary(libraryHandle);
}
