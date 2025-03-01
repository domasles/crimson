#include <Scene.h>

namespace crimson {
    SceneManager::~SceneManager() {
        if (m_CurrentScene) {
            m_CurrentScene->cleanup();
        }
    }

    bool SceneManager::registerScene(const std::string& name, std::shared_ptr<Scene> scene) {
        if (!scene) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot register scene: Scene pointer is null!");
            return false;
        }

        auto sceneIt = m_Scenes.find(name);

        if (sceneIt != m_Scenes.end()) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Scene %s is already registered!", name.c_str());
            return false;
        }

        m_Scenes[name] = std::move(scene);
        m_Scenes[name]->init();

        return true;
    }

    bool SceneManager::changeScene(const std::string& name) {
        auto sceneIt = m_Scenes.find(name);

        if (sceneIt == m_Scenes.end()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Scene %s not found!", name.c_str());
            return false;
        }

        std::shared_ptr<Scene> scene = sceneIt->second;

        if (!scene) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Scene %s is not initialized!", name.c_str());
            return false;
        }

        if (m_CurrentScene) {
            m_CurrentScene->cleanup();
        }

        m_CurrentScene = scene;
        m_CurrentScene->init();

        return true;
    }

    void SceneManager::update() {
        uint64_t currentTime = SDL_GetTicks();

        float deltaTime = (currentTime - m_LastFrameTime) / 1000.0f;
        m_LastFrameTime = currentTime;

        handleSceneTransition();

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
        if (m_CurrentScene) {
            return m_CurrentScene->getName();
        }

        return "No active scene!";
    }

    void SceneManager::handleSceneTransition() {
        if (m_PendingScene) {
            if (m_CurrentScene) {
                m_CurrentScene->cleanup();
            }

            m_CurrentScene = std::move(m_PendingScene);
            m_CurrentScene->init();
        }
    }
}
