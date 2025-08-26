#include <pch.h>

#include <utils/filesystem.h>

namespace launcher::utils::filesystem {
    const bool loadJSONFile(const std::string& filePath, json* jsonPtr) {
        std::ifstream file(filePath);
        
        if (!file.is_open()) {
            return false;
        }

        try {
            file >> *jsonPtr;
            return true;
        }

        catch (const std::exception& e) {
            return false;
        }
    }

    bool fileExists(const std::string& path) {
        try {
            return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
        }
        
        catch (...) {
            return false;
        }
    }

    bool directoryExists(const std::string& path) {
        try {
            return std::filesystem::exists(path) && std::filesystem::is_directory(path);
        }
        
        catch (...) {
            return false;
        }
    }

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

    std::vector<std::string> getSubdirectories(const std::string& path) {
        std::vector<std::string> subdirs;
        if (!directoryExists(path)) return subdirs;

        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_directory()) {
                    subdirs.push_back(entry.path().filename().string());
                }
            }
        }
        
        catch (const std::exception& e) {
            // Handle error silently
        }

        return subdirs;
    }
}