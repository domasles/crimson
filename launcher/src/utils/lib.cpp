#include <pch.h>

#include <utils/lib.h>
#include <utils/filesystem.h>

using namespace launcher::utils::filesystem;

namespace launcher::utils::lib {
    void* loadLibrary(const std::string& fileName) {
        const std::string& executableDir = getExecutableDirectory();
        const std::string& libraryPath = executableDir + "/" + fileName;

        #ifdef LAUNCHER_PLATFORM_WINDOWS
            int bufferSize = MultiByteToWideChar(CP_UTF8, 0, libraryPath.c_str(), -1, nullptr, 0);

            if (bufferSize == 0) {
                std::cerr << "Failed to calculate buffer size!" << std::endl;
                return nullptr;
            }

            std::vector<wchar_t> wideFileName(bufferSize);

            MultiByteToWideChar(CP_UTF8, 0, libraryPath.c_str(), -1, wideFileName.data(), bufferSize);

            HMODULE hLib = LoadLibraryW(wideFileName.data());

            if (hLib == nullptr) {
                std::cerr << "Failed to load the library!" << std::endl;
            }

            return hLib;

        #else
            void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);

            if (!handle) {
                std::cerr << "Failed to load the library: " << dlerror() << std::endl;
            }

            return handle;

        #endif
    }

    void* getFunction(void* libraryHandle, const std::string& functionName) {
        #ifdef LAUNCHER_PLATFORM_WINDOWS
            return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(libraryHandle), functionName.c_str()));
        #else
            return dlsym(libraryHandle, functionName.c_str());
        #endif
    }

    void unloadLibrary(void* libraryHandle) {
        #ifdef LAUNCHER_PLATFORM_WINDOWS
            FreeLibrary(static_cast<HMODULE>(libraryHandle));
        #else
            dlclose(libraryHandle);
        #endif
    }
}