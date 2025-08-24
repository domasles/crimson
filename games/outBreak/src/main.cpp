#include <pch.h>

#include <Engine.h>

#include <componentRegistrator.h>
#include <sceneRegistrator.h>

using namespace outBreak;

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

constexpr bool RESIZABLE = true;

Core& engineCore = getCore();

GAME_API bool start() {
    if (!engineCore.init("outBreak", "OutBreak", WINDOW_WIDTH, WINDOW_HEIGHT, RESIZABLE)) {
        return false;
    }

    // Set white background
    engineCore.setBackgroundColor(Color(1.0f, 1.0f, 1.0f, 1.0f));

    registrateComponents();
    registrateScenes();

    return true;
}
