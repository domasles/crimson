#include <pch.h>

#include <utils/filesystem.h>

namespace launcher::utils::filesystem {
    std::string getExecutableDirectory() {
        #ifdef LAUNCHER_PLATFORM_WINDOWS
            char buffer[MAX_PATH];

            if (GetModuleFileNameA(NULL, buffer, MAX_PATH)) {
                std::string executablePath(buffer);

                size_t pos = executablePath.find_last_of("\\/");

                return executablePath.substr(0, pos);
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