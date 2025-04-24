#pragma once

using json = nlohmann::json;

namespace engine::utils::filesystem {
    const bool loadJSONFile(const std::string& filePath, json* jsonPtr);

    const std::string& getBasePath();
    const std::string& getGamePath();

    std::string getParentPath(const std::string& fullPath);
    std::string getFileName(const std::string& fullPath);
}
