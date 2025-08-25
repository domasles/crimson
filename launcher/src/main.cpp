#include <pch.h>

#ifdef LAUNCHER_PLATFORM_EMSCRIPTEN
    extern "C" {
        bool start();
        void internalUpdate(std::function<void()> customUpdateFunc);
    }

    int main() {
        if (!start()) {
            std::cerr << "Game start failed" << std::endl;
            return -1;
        }

        internalUpdate(nullptr);
        return 0;
    }
#else
    #include <utils/game_selector.h>
    #include <utils/filesystem.h>
    #include <utils/lib.h>

    #include <wx/wx.h>
    
    class GameLauncherApp : public wxApp {
        public:
            bool OnInit() override {
                return true;
            }
    };
    
    wxIMPLEMENT_APP_NO_MAIN(GameLauncherApp);

    using namespace launcher::utils::filesystem;
    using namespace launcher::utils::lib;

    using namespace launcher::utils::game_selector;

    int main(int argc, char* argv[]) {
        wxInitialize(argc, argv);

        auto availableGames = scanAvailableGames();
        auto selectedGame = selectGame(availableGames);

        if (!selectedGame) {
            std::cerr << "No game selected or available." << std::endl;
            wxUninitialize();
            return -1;
        }

        std::string fileName = selectedGame->dllPath;

        wxUninitialize();

        void* libraryHandle = loadLibrary(fileName);
        if (!libraryHandle) return -1;
    
        auto awakeFunc = reinterpret_cast<bool(*)()>(getFunction(libraryHandle, "awake"));
        auto startFunc = reinterpret_cast<bool(*)()>(getFunction(libraryHandle, "start"));

        auto customUpdateFunc = reinterpret_cast<void(*)()>(getFunction(libraryHandle, "update"));
        auto internalUpdateFunc = reinterpret_cast<void(*)(std::function<void()> customUpdateFunc)>(getFunction(libraryHandle, "internalUpdate"));

        if (!internalUpdateFunc) {
            std::cerr << "Failed to load internalUpdate function from game library" << std::endl;
            unloadLibrary(libraryHandle);
            return -1;
        }

        if (awakeFunc && !awakeFunc()) {
            std::cerr << "Game awake failed" << std::endl;
            unloadLibrary(libraryHandle);
            return -1;
        }

        if (!startFunc) {
            std::cerr << "Game entry point 'start' not found." << std::endl;
            unloadLibrary(libraryHandle);
            return -1;
        }

        if (!startFunc()) {
            std::cerr << "Game start failed" << std::endl;
            unloadLibrary(libraryHandle);
            return -1;
        }

        internalUpdateFunc(customUpdateFunc);
        unloadLibrary(libraryHandle);

        return 0;
    }

    #ifdef _WIN32
        #ifdef NDEBUG
            #include <windows.h>

            int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
                return main(__argc, __argv);
            }
        #endif
    #endif
#endif
