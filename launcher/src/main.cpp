#include <pch.h>

#ifdef LAUNCHER_PLATFORM_EMSCRIPTEN
    extern "C" {
        bool start();
        void internalUpdate(std::function<void()> customUpdateFunc);
    }

    int main() {
        if (!start()) {
            std::println(stderr, "Game start failed");
            return -1;
        }

        internalUpdate(nullptr);
        return 0;
    }
#else
    #include <utils/game_selector.h>
    #include <utils/filesystem.h>
    #include <utils/lib.h>

    using namespace launcher::utils::filesystem;
    using namespace launcher::utils::lib;
    using namespace launcher::utils::game_selector;

    int main(int argc, char* argv[]) {

        auto availableGames = scanAvailableGames();
        auto selectedGame = selectGame(availableGames);

        if (!selectedGame) {
            std::println(stderr, "No game selected or available");
            return -1;
        }

        std::string fileName = selectedGame->libPath;

        void* libraryHandle = loadLibrary(fileName);
        if (!libraryHandle) return -1;
    
        auto awakeFunc = reinterpret_cast<bool(*)()>(getFunction(libraryHandle, "awake"));
        auto startFunc = reinterpret_cast<bool(*)()>(getFunction(libraryHandle, "start"));

        auto customUpdateFunc = reinterpret_cast<void(*)()>(getFunction(libraryHandle, "update"));
        auto internalUpdateFunc = reinterpret_cast<void(*)(std::function<void()> customUpdateFunc)>(getFunction(libraryHandle, "internalUpdate"));

        if (!internalUpdateFunc) {
            std::println(stderr, "Failed to load internalUpdate function from game library");
            unloadLibrary(libraryHandle);
            return -1;
        }

        if (awakeFunc && !awakeFunc()) {
            std::println(stderr, "Game awake failed");
            unloadLibrary(libraryHandle);
            return -1;
        }

        if (!startFunc) {
            std::println(stderr, "Game entry point 'start' not found");
            unloadLibrary(libraryHandle);
            return -1;
        }

        if (!startFunc()) {
            std::println(stderr, "Game start failed");
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
