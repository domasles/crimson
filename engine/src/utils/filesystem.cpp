#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Core.h>

using namespace engine::utils::logger;

namespace engine::utils::filesystem {
    const bool loadJSONFile(const std::string& filePath, json* jsonPtr) {
        std::ifstream file(filePath);
        
        if (!file.is_open()) {
            Logger::engine_error("Failed to open file: {}", filePath);
            return false;
        }

        try {
            file >> *jsonPtr;
            return true;
        }

        catch (const std::exception& e) {
            Logger::engine_error("JSON parsing error: {}", e.what());
            return false;
        }
    }

    const std::string& getBasePath() {
        static std::string basePath = SDL_GetBasePath();
        return basePath;
    }
    
    const std::string& getGamePath() {
        static std::string path = std::format("{}/games/{}", getBasePath(), Core::getInstance().getName());
        return path;
    }

    std::string getParentPath(const std::string& fullPath) {
        return std::filesystem::path(fullPath).parent_path().string();
    }

    std::string getFileName(const std::string& fullPath) {
        return std::filesystem::path(fullPath).filename().string();
    }
}
