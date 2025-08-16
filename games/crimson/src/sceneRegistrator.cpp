#include <pch.h>

#include <scenes/Level1.h>

#include <Scene.h>

#include <sceneRegistrator.h>

namespace crimson {
    void registrateScenes() {
        createScene<Level1>("Level1");
        switchToScene("Level1");
    }
}
