#include <pch.h>

#include <utils/filesystem.h>

#include <Core.h>

namespace engine::utils::filesystem {
    const std::string& getBasePath() {
        static std::string basePath = SDL_GetBasePath();
        return basePath;
    }

    const std::string& getGamePath() {
        static std::string path = getBasePath() + "games/" + Core::getInstance().getName() + "/";
        return path;
    }
}