#include <pch.h>

#include <Scene.h>

namespace crimson {
    SceneManager& SceneManager::getInstance() {
        static SceneManager* instance = nullptr;
        static std::once_flag flag;

        std::call_once(flag, []() { instance = new SceneManager(); });

        return *instance;
    }

    bool SceneManager::registerScene(const std::string& name, std::shared_ptr<Scene> scene) {
        if (!scene) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot register scene: Scene pointer is null!");
            return false;
        }

        if (m_Scenes.find(name) != m_Scenes.end()) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Scene %s is already registered!", name.c_str());
            return false;
        }

        m_Scenes[name] = std::move(scene);
        
        return true;
    }

    bool SceneManager::changeScene(const std::string& name) {
        auto sceneIt = m_Scenes.find(name);

        if (sceneIt == m_Scenes.end()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Scene %s not found!", name.c_str());
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

    void SceneManager::update() {
        uint64_t currentTime = SDL_GetTicks();

        float deltaTime = (currentTime - m_LastFrameTime) / 1000.0f;

        m_LastFrameTime = currentTime;

        if (m_CurrentScene) {
            m_CurrentScene->update(deltaTime);
        }
    }

    void SceneManager::render() {
        if (m_CurrentScene) {
            m_CurrentScene->render();
        }
    }

    std::string SceneManager::getCurrentSceneName() const {
        return m_CurrentScene ? m_CurrentScene->getName() : "No active scene!";
    }
}
