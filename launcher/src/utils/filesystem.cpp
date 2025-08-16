#include <pch.h>

#include <utils/filesystem.h>

namespace launcher::utils::filesystem {
    std::string getExecutableDirectory() {
        #ifdef LAUNCHER_PLATFORM_WINDOWS
            char buffer[MAX_PATH];
            
            if (GetModuleFileNameA(nullptr, buffer, MAX_PATH)) {
                std::string executablePath(buffer);

                if (auto pos = executablePath.find_last_of("\\/"); pos != std::string::npos) {
                    return executablePath.substr(0, pos);
                }
            }

            return "";

        #elif defined(LAUNCHER_PLATFORM_MACOS)
            char result[1024];
            uint32_t size = sizeof(result);

            if (_NSGetExecutablePath(result, &size) == 0) {
                std::string executablePath(result);
                return std::string(dirname(&executablePath[0]));
            }

            return "";

        #else
            char result[1024];

            ssize_t count = readlink("/proc/self/exe", result, sizeof(result));

            if (count != -1) {
                result[count] = '\0';

                std::string executablePath(result);

                return std::string(dirname(&executablePath[0]));
            }

            return "";

        #endif
    }
}