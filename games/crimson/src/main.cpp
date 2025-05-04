#include <pch.h>

#include <entities/Player.h>

#include <Scene.h>
#include <Input.h>
#include <Core.h>

#include <sceneRegistrator.h>

using namespace engine;
using namespace crimson;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr bool RESIZABLE = true;

Core& engineCore = Core::getInstance();

GAME_API bool init() {
    if (!engineCore.init("crimson", "Crimson", WINDOW_WIDTH, WINDOW_HEIGHT, RESIZABLE)) {
        return false;
    }

    registrateScenes();
    return true;
}
