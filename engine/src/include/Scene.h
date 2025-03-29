#pragma once

#include <Core.h>

namespace engine {
    class Scene {
        public:
            virtual ~Scene() = default;

            virtual void init() = 0;
            virtual void update(float deltaTime) = 0;
            virtual void render() = 0;

            void setInitialized(bool value) { m_Initialized = value; }

            const bool getInitialized() const { return m_Initialized; }
            const std::string& getName() const { return m_Name; }

        protected:
            Core& m_EngineCore = Core::getInstance();

            bool m_Initialized = false;

            std::string m_Name;
    };

    class SceneManager {
        public:
            static SceneManager& getInstance();

            const bool registerScene(const std::string& name, std::shared_ptr<Scene> scene);
            const bool changeScene(const std::string& name);

            const bool update();
            const bool render();

            const std::string& getCurrentSceneName() const;

        private:
            SceneManager() : m_LastFrameTime(SDL_GetTicksNS()) {}
            ~SceneManager() = default;

            SceneManager(const SceneManager&) = delete;
            SceneManager& operator=(const SceneManager&) = delete;

            std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;

            std::shared_ptr<Scene> m_CurrentScene = nullptr;
            std::shared_ptr<Scene> m_PreviousScene = nullptr;

            uint64_t m_LastFrameTime;
    };
}
