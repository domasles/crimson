#include <pch.h>

#include <utils/game_selector.h>
#include <utils/filesystem.h>

#ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
    #include <wx/wx.h>
    #include <wx/choicdlg.h>
#endif

namespace launcher::utils::game_selector {
    bool loadGameManifest(const std::string& manifestPath, GameInfo& gameInfo) {
        json jsonData;

        if (!filesystem::loadJSONFile(manifestPath, &jsonData)) return false;
        const auto& json = jsonData;

        try {
            if (json.contains("display_name") && json["display_name"].is_string()) {
                gameInfo.displayName = json["display_name"];
            }

            if (json.contains("version") && json["version"].is_string()) {
                gameInfo.version = json["version"];
            }

            if (json.contains("dll_name") && json["dll_name"].is_string()) {
                gameInfo.dllName = json["dll_name"];
            }

            if (json.contains("so_name") && json["so_name"].is_string()) {
                gameInfo.soName = json["so_name"];
            }

            if (json.contains("dylib_name") && json["dylib_name"].is_string()) {
                gameInfo.dylibName = json["dylib_name"];
            }

            gameInfo.manifestPath = manifestPath;

            return !gameInfo.displayName.empty()
                && (!gameInfo.dllName.empty() || !gameInfo.soName.empty() || !gameInfo.dylibName.empty());
        }

        catch (const std::exception& e) {
            return false;
        }
    }

    bool validateGameDll(const GameInfo& gameInfo) {
        return filesystem::fileExists(gameInfo.libPath);
    }

    std::vector<GameInfo> scanAvailableGames() {
        std::vector<GameInfo> games;

        #ifdef LAUNCHER_PLATFORM_WINDOWS
            std::string gamesDir = filesystem::getExecutableDirectory() + "\\games";
        #else
            std::string gamesDir = filesystem::getExecutableDirectory() + "/games";
        #endif

        std::cout << "Scanning games in directory: " << gamesDir << std::endl;

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
                    if (loadGameManifest(manifestPath, gameInfo)) {
                        #ifdef LAUNCHER_PLATFORM_WINDOWS
                            gameInfo.libPath = gamesDir + "\\" + subdir + "\\" + gameInfo.dllName;
                        #elif defined(LAUNCHER_PLATFORM_MACOS)
                            gameInfo.libPath = gamesDir + "/" + subdir + "/" + gameInfo.dylibName;
                        #else
                            gameInfo.libPath = gamesDir + "/" + subdir + "/" + gameInfo.soName;
                        #endif

                        if (validateGameDll(gameInfo)) {
                            games.push_back(gameInfo);
                        }
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
