#include <pch.h>
#include <utils/game_selector.h>
#include <utils/filesystem.h>

#ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
    #include <wx/wx.h>
    #include <wx/choicdlg.h>
#endif

namespace launcher::utils::game_selector {
    bool loadGameManifest(const std::string& manifestPath, GameInfo& gameInfo) {
        std::ifstream file(manifestPath);
        if (!file.is_open()) return false;

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        auto findValue = [&content](const std::string& key) -> std::string {
            std::string searchKey = "\"" + key + "\"";

            size_t keyPos = content.find(searchKey);
            if (keyPos == std::string::npos) return "";

            size_t colonPos = content.find(":", keyPos);
            if (colonPos == std::string::npos) return "";

            size_t startQuote = content.find("\"", colonPos);
            if (startQuote == std::string::npos) return "";

            startQuote++;

            size_t endQuote = content.find("\"", startQuote);
            if (endQuote == std::string::npos) return "";

            return content.substr(startQuote, endQuote - startQuote);
        };

        gameInfo.displayName = findValue("display_name");
        gameInfo.version = findValue("version");
        gameInfo.dllName = findValue("dll_name");
        gameInfo.manifestPath = manifestPath;

        return !gameInfo.displayName.empty() && !gameInfo.dllName.empty();
    }

    bool validateGameDll(const GameInfo& gameInfo) {
        return filesystem::fileExists(gameInfo.dllPath);
    }

    std::vector<GameInfo> scanAvailableGames() {
        std::vector<GameInfo> games;
        std::string gamesDir = "games";

        if (!filesystem::directoryExists(gamesDir)) {
            return games;
        }

        auto subdirs = filesystem::getSubdirectories(gamesDir);

        for (const auto& subdir : subdirs) {
            std::string manifestPath = gamesDir + "/" + subdir + "/config/Manifest.json";

            if (filesystem::fileExists(manifestPath)) {
                GameInfo gameInfo;
                gameInfo.name = subdir;

                if (loadGameManifest(manifestPath, gameInfo)) {
                    gameInfo.dllPath = gamesDir + "/" + subdir + "/" + gameInfo.dllName;

                    if (validateGameDll(gameInfo)) {
                        games.push_back(gameInfo);
                    }
                }
            }
        }

        return games;
    }

    std::optional<GameInfo> selectGame(const std::vector<GameInfo>& games) {
        if (games.empty()) {
            #ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
                wxMessageBox("No games found!", "Error", wxOK | wxICON_ERROR);
            #else
                std::cerr << "No games found!" << std::endl;
            #endif

            return std::nullopt;
        }

        if (games.size() == 1) return games[0];

        #ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
            wxArrayString choices;

            for (const auto& game : games) {
                choices.Add(wxString::FromUTF8(game.displayName + " (v" + game.version + ")"));
            }

            wxSingleChoiceDialog dialog(
                nullptr,
                "Select a game to play:",
                "Game Selector",
                choices
            );

            dialog.SetSelection(0);

            if (dialog.ShowModal() == wxID_OK) {
                int selection = dialog.GetSelection();
                if (selection >= 0 && selection < static_cast<int>(games.size())) return games[selection];
            }

            return std::nullopt;
        #else
            std::cerr << "Game selection not supported in WASM builds" << std::endl;
            return std::nullopt;
        #endif
    }
}
