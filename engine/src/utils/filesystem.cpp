#include <pch.h>

#include <utils/filesystem.h>

#include <Core.h>

namespace engine::utils::filesystem {
    const bool loadJSONFile(const std::string& filePath, json* jsonPtr) {
        std::ifstream file(filePath);

        if (!file.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open file: %s", filePath.c_str());
            return false;
        }

        try {
            file >> *jsonPtr;
            return true;
        }

        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSON parsing error: %s", e.what());
            return false;
        }
    }

    const std::string& getBasePath() {
        static std::string basePath = SDL_GetBasePath();
        return basePath;
    }

    const std::string& getGamePath() {
        static std::string path = getBasePath() + "games/" + Core::getInstance().getName();
        return path;
    }

    std::string getParentPath(const std::string& fullPath) {
        std::filesystem::path path(fullPath);
        std::string parentPath = path.parent_path().string();

        return parentPath;
    }

    std::string getFileName(const std::string& fullPath) {
        std::filesystem::path path(fullPath);
        std::string fileName = path.filename().string();

        return fileName;
    }
}