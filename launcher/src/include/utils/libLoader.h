#pragma once

namespace launcher::utils::libLoader {
    void* loadLibrary(const std::string& fileName);
    void* getFunction(void* libraryHandle, const std::string& functionName);

    void unloadLibrary(void* libraryHandle);
}
