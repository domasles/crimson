#pragma once

using json = nlohmann::json;

namespace launcher::utils::filesystem {
    const bool loadJSONFile(const std::string& filePath, json* jsonPtr);

    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);

    std::string getExecutableDirectory();
    std::vector<std::string> getSubdirectories(const std::string& path);
}
