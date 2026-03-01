#include <pch.h>

#include <scenes/StartScene.h>
#include <scenes/GameScene.h>

#include <Engine.h>

#include <sceneRegistrator.h>

namespace outBreak {
    void registrateScenes() {
        createScene<StartScene>("StartScene");
        createScene<GameScene>("GameScene");

        switchToScene("StartScene");
    }
}
