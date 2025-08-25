#pragma once

#include <string>
#include <vector>

namespace launcher::utils::filesystem {
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);

    std::string getExecutableDirectory();
    std::vector<std::string> getSubdirectories(const std::string& path);
}
