#include <pch.h>

#include <Engine.h>

#include <componentRegistrator.h>
#include <sceneRegistrator.h>

using namespace outBreak;

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

constexpr bool RESIZABLE = true;
constexpr bool VSYNC = true;

GAME_API bool start() {
    if (!getCore().init("outBreak", "OutBreak", WINDOW_WIDTH, WINDOW_HEIGHT, RESIZABLE, VSYNC)) {
        return false;
    }

    Gizmos::enableAll();

    registrateComponents();
    registrateScenes();

    return true;
}
