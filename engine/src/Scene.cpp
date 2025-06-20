#include <pch.h>

#include <utils/logger.h>

#include <Scene.h>

using namespace engine::utils::logger;

namespace engine {
    SceneManager& SceneManager::getInstance() {
        try {
            static SceneManager& instance = *new SceneManager();
            return instance;
        }

        catch (const std::bad_alloc& e) {
            Logger::error("Memory allocation failed: %s", e.what());
        }

        static SceneManager fallbackInstance;
        return fallbackInstance;
    }

    const bool SceneManager::registerScene(const std::string& name, std::shared_ptr<Scene> scene) {
        if (!scene) {
            Logger::error("Cannot register scene: Scene pointer is null!");
            return false;
        }

        if (m_Scenes.find(name) != m_Scenes.end()) {
            Logger::warn("Scene %s is already registered!", name.c_str());
            return false;
        }

        m_Scenes[name] = std::move(scene);
        return true;
    }

    const bool SceneManager::unregisterScene(const std::string& name) {
        auto it = m_Scenes.find(name);

        if (it == m_Scenes.end()) {
            Logger::warn("Scene %s not found. Cannot unregister!", name.c_str());
            return false;
        }

        m_Scenes.erase(it);
        return true;
    }

    const bool SceneManager::changeScene(const std::string& name) {
        auto sceneIt = m_Scenes.find(name);

        if (sceneIt == m_Scenes.end()) {
            Logger::error("Scene %s not found!", name.c_str());
            return false;
        }

        std::shared_ptr<Scene> newScene = sceneIt->second;

        if (!newScene->getInitialized()) {
            newScene->setInitialized(true);
            newScene->init();
        }

        m_PreviousScene = m_CurrentScene;
        m_CurrentScene = newScene;

        return true;
    }

    const bool SceneManager::update() {
        uint64_t currentTime = SDL_GetTicks();

        float deltaTime = (currentTime - m_LastFrameTime) / 1000.0f;

        m_LastFrameTime = currentTime;

        if (!m_CurrentScene) {
            Logger::error("No current scene is found or selected!");
            return false;
        }

        m_CurrentScene->update(deltaTime);
        return true;
    }

    const bool SceneManager::render() {
        if (!m_CurrentScene) {
            Logger::error("No current scene is found or selected!");
            return false;
        }

        m_CurrentScene->render();
        return true;
    }

    const std::string& SceneManager::getCurrentSceneName() const {
        if (m_CurrentScene) {
            return m_CurrentScene->getName();
        }

        return m_FallbackCurrentSceneText;
    }
}
