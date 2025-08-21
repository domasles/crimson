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
            Logger::engine_error("Memory allocation failed: {}", e.what());
        }

        static SceneManager fallbackInstance;
        return fallbackInstance;
    }

    const bool SceneManager::registerScene(const std::string& name, std::shared_ptr<Scene> scene) {
        if (!scene) {
            Logger::engine_error("Cannot register scene: Scene pointer is null!");
            return false;
        }
        
        if (m_Scenes.find(name) != m_Scenes.end()) {
            Logger::engine_warn("Scene {} is already registered!", name);
            return false;
        }
        
        m_Scenes[name] = std::move(scene);

        ENGINE_LOG_INIT(("Scene: " + name).c_str());

        return true;
    }

    const bool SceneManager::unregisterScene(const std::string& name) {
        auto it = m_Scenes.find(name);
        
        if (it == m_Scenes.end()) {
            Logger::engine_warn("Scene {} not found. Cannot unregister!", name);
            return false;
        }

        m_Scenes.erase(it);
        return true;
    }

    const bool SceneManager::changeScene(const std::string& name) {
        auto sceneIt = m_Scenes.find(name);

        if (sceneIt == m_Scenes.end()) {
            Logger::engine_error("Scene {} not found!", name);
            return false;
        }

        std::shared_ptr<Scene> newScene = sceneIt->second;

        m_PreviousScene = m_CurrentScene;
        m_CurrentScene = newScene;

        if (!newScene->getInitialized()) {
            newScene->setInitialized(true);
            newScene->init();
        }

        return true;
    }

    const bool SceneManager::update() {
        uint64_t currentTime = SDL_GetTicks();

        float deltaTime = (currentTime - m_LastFrameTime) / 1000.0f;

        m_LastFrameTime = currentTime;

        if (!m_CurrentScene) {
            Logger::engine_error("No current scene is found or selected!");
            return false;
        }

        m_CurrentScene->update(deltaTime);
        return true;
    }

    const bool SceneManager::render() {
        if (!m_CurrentScene) {
            Logger::engine_error("No current scene is found or selected!");
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

    std::shared_ptr<Scene> SceneManager::getCurrentScene() const {
        return m_CurrentScene;
    }
}
