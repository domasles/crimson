#include <pch.h>

#include <scenes/Level1.h>

#include <Scene.h>

#include <sceneRegistrator.h>

namespace crimson {
    void registrateScenes() {
        SceneManager& sceneManager = SceneManager::getInstance();

        std::shared_ptr<Level1> m_TestScene = std::make_shared<Level1>();

        if (!sceneManager.registerScene(m_TestScene->getName(), m_TestScene)) return;
        if (!sceneManager.changeScene(m_TestScene->getName())) return;
    }
}
