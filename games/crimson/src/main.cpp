#include <pch.h>

#include <entities/Player.h>

#include <Engine.h>

#include <componentRegistrator.h>
#include <sceneRegistrator.h>

using namespace engine;
using namespace crimson;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr bool RESIZABLE = true;

Core& engineCore = getCore();

GAME_API bool start() {
    if (!engineCore.init("crimson", "Crimson", WINDOW_WIDTH, WINDOW_HEIGHT, RESIZABLE)) {
        return false;
    }

    registrateComponents();
    registrateScenes();

    return true;
}
