#include <pch.h>

#include <Scene.h>

#include <sceneRegistrator.h>

namespace crimson {
    void registrateScenes() {
        SceneManager& sceneManager = SceneManager::getInstance();

        std::shared_ptr<TestScene> m_TestScene = std::make_shared<TestScene>();

        if (!sceneManager.registerScene(m_TestScene->getName(), m_TestScene)) return;
        if (!sceneManager.changeScene(m_TestScene->getName())) return;
    }
}