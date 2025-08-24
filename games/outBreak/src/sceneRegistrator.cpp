#include <pch.h>

#include <scenes/MainScene.h>

#include <Engine.h>

#include <sceneRegistrator.h>

namespace outBreak {
    void registrateScenes() {
        createScene<MainScene>("MainScene");
        switchToScene("MainScene");
    }
}
