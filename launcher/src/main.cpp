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

    if (!libraryHandle) return -1;

    auto init = reinterpret_cast<bool(*)()>(getFunction(libraryHandle, "init"));
    auto customUpdate = reinterpret_cast<void(*)()>(getFunction(libraryHandle, "update"));
    auto internalUpdate = reinterpret_cast<void(*)(std::function<void()> customUpdateFunc)>(getFunction(libraryHandle, "internalUpdate"));

    if (init()) {
        if (customUpdate) {
            internalUpdate(customUpdate);
        }

        else {
            internalUpdate(nullptr);
        }
    }

    unloadLibrary(libraryHandle);
}
