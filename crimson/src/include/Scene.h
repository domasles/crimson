#pragma once

namespace crimson {
    class Scene {
        public:
            virtual ~Scene() = default;

            virtual void init() = 0;
            virtual void update(float deltaTime) = 0;
            virtual void render() = 0;
            virtual void cleanup() = 0;

            std::string getName() const { return m_Name; }

        protected:
            std::string m_Name;
    };

    class SceneManager {
        public:
            static SceneManager& getInstance() {
                static SceneManager instance;
                return instance;
            }

            bool registerScene(const std::string& name, std::shared_ptr<Scene> scene);
            bool changeScene(const std::string& name);

            void update();
            void render();

            std::string getCurrentSceneName() const;

        private:
            SceneManager() : m_LastFrameTime(SDL_GetTicks()) {}
            ~SceneManager();

            SceneManager(const SceneManager&) = delete;
            SceneManager& operator=(const SceneManager&) = delete;

            void handleSceneTransition();

            std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;

            std::shared_ptr<Scene> m_CurrentScene = nullptr;
            std::shared_ptr<Scene> m_PendingScene = nullptr;

            uint64_t m_LastFrameTime;
    };
}
