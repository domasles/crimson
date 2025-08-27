#pragma once

#include <string>
#include <vector>
#include <optional>

namespace launcher::utils::game_selector {
    struct GameInfo {
        std::string name;
        std::string displayName;
        std::string version;
        std::string dllName;
        std::string soName;
        std::string dylibName;
        std::string manifestPath;
        std::string libPath;
    };

    bool loadGameManifest(const std::string& manifestPath, GameInfo& gameInfo);
    bool validateGameDll(const GameInfo& gameInfo);

    std::vector<GameInfo> scanAvailableGames();
    std::optional<GameInfo> selectGame(const std::vector<GameInfo>& games);
}
