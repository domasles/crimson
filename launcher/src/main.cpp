#include <pch.h>

#include <utils/filesystem.h>
#include <utils/lib.h>

using namespace launcher::utils::filesystem;
using namespace launcher::utils::lib;

int main() {
    #ifdef LAUNCHER_PLATFORM_WINDOWS
        const std::string fileName = "games/crimson/crimson.dll";
    #else
        const std::string fileName = "games/crimson/libcrimson.so";
    #endif

    void* libraryHandle = loadLibrary(fileName);

    if (!libraryHandle) {
        return -1;
    }
    
    auto awakeFunc = reinterpret_cast<bool(*)()>(getFunction(libraryHandle, "awake"));
    auto startFunc = reinterpret_cast<bool(*)()>(getFunction(libraryHandle, "start"));

    auto customUpdateFunc = reinterpret_cast<void(*)()>(getFunction(libraryHandle, "update"));
    auto internalUpdateFunc = reinterpret_cast<void(*)(std::function<void()> customUpdateFunc)>(getFunction(libraryHandle, "internalUpdate"));

    if (!internalUpdateFunc) {
        std::cerr << "Failed to load internalUpdate function from game library" << std::endl;
        unloadLibrary(libraryHandle);
        return -1;
    }

    if (awakeFunc && !awakeFunc()) {
        std::cerr << "Game awake failed" << std::endl;
        unloadLibrary(libraryHandle);
        return -1;
    }

    if (!startFunc) {
        std::cerr << "Game entry point 'start' not found." << std::endl;
        unloadLibrary(libraryHandle);
        return -1;
    }

    if (!startFunc()) {
        std::cerr << "Game start failed" << std::endl;
        unloadLibrary(libraryHandle);
        return -1;
    }

    internalUpdateFunc(customUpdateFunc);
    unloadLibrary(libraryHandle);

    return 0;
}
