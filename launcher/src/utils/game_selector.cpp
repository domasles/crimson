#include <pch.h>

#include <utils/game_selector.h>
#include <utils/filesystem.h>

#ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
    #include <inter.h>
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

        std::println("Scanning games in directory: {}", gamesDir);

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

        return games;
    }

    #ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
        static std::optional<GameInfo> showNuklearPicker(const std::vector<GameInfo>& games) {
            constexpr int WIN_W = 440;
            constexpr int WIN_H = 360;

            SDL_Window* window   = nullptr;
            SDL_Renderer* renderer = nullptr;

            if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
                std::println(stderr, "SDL_Init failed: {}", SDL_GetError());
                return std::nullopt;
            }

            if (!SDL_CreateWindowAndRenderer( "Select a Game", WIN_W, WIN_H, SDL_WINDOW_HIGH_PIXEL_DENSITY, &window, &renderer)) {
                std::println(stderr, "SDL_CreateWindowAndRenderer failed: {}", SDL_GetError());
                SDL_Quit();
                return std::nullopt;
            }

            SDL_SetWindowResizable(window, false);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

            const float dpiScale = SDL_GetWindowDisplayScale(window);
            SDL_SetRenderScale(renderer, dpiScale, dpiScale);

            struct nk_context* ctx = nk_sdl_init(window, renderer, nk_sdl_allocator());

            struct nk_font* fontBig = nullptr;
            struct nk_font* fontReg = nullptr;

            {
                struct nk_font_atlas* atlas = nk_sdl_font_stash_begin(ctx);
                struct nk_font_config config = nk_font_config(0);

                config.oversample_h = 4;
                config.oversample_v = 4;

                fontBig = nk_font_atlas_add_from_memory(atlas, inter_ttf, inter_ttf_len, 25.0f, &config);
                fontReg = nk_font_atlas_add_from_memory(atlas, inter_ttf, inter_ttf_len, 20.0f, &config);

                if (!fontBig) fontBig = nk_font_atlas_add_default(atlas, 25.0f, &config);
                if (!fontReg) fontReg = nk_font_atlas_add_default(atlas, 20.0f, &config);

                nk_sdl_font_stash_end(ctx);

                fontBig->handle.height /= dpiScale;
                fontReg->handle.height /= dpiScale;

                nk_style_set_font(ctx, &fontBig->handle);
            }

            std::vector<std::string> labels;
            labels.reserve(games.size());

            for (const auto& g : games) {
                labels.push_back(g.displayName + "  (v" + g.version + ")");
            }

            int selected = 0;
            std::optional<GameInfo> result;
            bool running = true;

            nk_input_begin(ctx);

            while (running) {
                SDL_Event evt;

                while (SDL_PollEvent(&evt)) {
                    if (evt.type == SDL_EVENT_QUIT) running = false;

                    SDL_ConvertEventToRenderCoordinates(renderer, &evt);
                    nk_sdl_handle_event(ctx, &evt);
                }

                nk_input_end(ctx);

                const float W = (float)WIN_W / dpiScale;
                const float H = (float)WIN_H / dpiScale;

                if (nk_begin(ctx, "Select a Game", nk_rect(0, 0, W, H), NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
                    const float btnRowH  = 44.0f;
                    const float padding  = 10.0f;
                    const float listH    = H - 60.0f - btnRowH - padding;

                    nk_layout_row_dynamic(ctx, listH, 1);

                    // Only reserve scrollbar space when items actually overflow
                    const float itemH = 30.0f;
                    const nk_flags groupFlags = NK_WINDOW_BORDER | (static_cast<int>(games.size()) * itemH <= listH ? NK_WINDOW_NO_SCROLLBAR : 0);

                    if (nk_group_begin(ctx, "GameList", groupFlags)) {
                        nk_style_push_font(ctx, &fontReg->handle);
                        nk_layout_row_dynamic(ctx, itemH, 1);

                        for (int i = 0; i < static_cast<int>(games.size()); ++i) {
                            nk_bool active = (selected == i) ? nk_true : nk_false;
                            if (nk_selectable_label(ctx, labels[i].c_str(), NK_TEXT_LEFT, &active) && active) selected = i;
                        }

                        nk_style_pop_font(ctx);
                        nk_group_end(ctx);
                    }

                    nk_layout_row_dynamic(ctx, btnRowH - 10.0f, 2);

                    if (nk_button_label(ctx, "Launch")) {
                        result = games[static_cast<size_t>(selected)];
                        running = false;
                    }

                    if (nk_button_label(ctx, "Cancel")) running = false;
                }

                nk_end(ctx);

                SDL_SetRenderDrawColorFloat(renderer, 0.10f, 0.11f, 0.13f, 1.0f);
                SDL_RenderClear(renderer);

                nk_sdl_render(ctx, NK_ANTI_ALIASING_ON);

                SDL_RenderPresent(renderer);

                nk_input_begin(ctx);
            }

            nk_sdl_shutdown(ctx);

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();

            return result;
        }
    #endif

    std::optional<GameInfo> selectGame(const std::vector<GameInfo>& games) {
        if (games.empty()) {
            std::println(stderr, "No games found!");
            return std::nullopt;
        }

        if (games.size() == 1) return games[0];

        #ifndef LAUNCHER_PLATFORM_EMSCRIPTEN
            return showNuklearPicker(games);
        #endif
    }
}
