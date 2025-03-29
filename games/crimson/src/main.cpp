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

constexpr bool FULLSCREEN = false;

Core& engineCore = Core::getInstance();

GAME_API void init() {
    if (!engineCore.init("crimson", "Crimson", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN)) return;
    registrateScenes();
}
