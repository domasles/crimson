#pragma once

#include <Entity.h>
#include <Input.h>
#include <Core.h>
#include <Map.h>

namespace engine {
    class Scene {
        public:
            virtual ~Scene() = default;

            virtual void init() = 0;
            virtual void update(float deltaTime) = 0;
            virtual void render() = 0;

            void setInitialized(bool value) { m_Initialized = value; }

            bool getInitialized() const { return m_Initialized; }
            const std::string& getName() const { return m_Name; }

            template<typename T, typename... Args>

            T* createEntity(Args&&... args) {
                auto entity = std::make_unique<T>(std::forward<Args>(args)...);

                T* ptr = entity.get();
                m_Entities.push_back(std::move(entity));

                return ptr;
            }

            template<typename T>

            T* findEntity() {
                for (auto& entity : m_Entities) {
                    if (auto* typed = dynamic_cast<T*>(entity.get())) {
                        return typed;
                    }
                }

                return nullptr;
            }

            void removeEntity(Entity* entity) {
                m_Entities.erase(
                    std::remove_if(m_Entities.begin(), m_Entities.end(),
                        [entity](const auto& ptr) { return ptr.get() == entity; }),
                    m_Entities.end());
            }

            size_t getEntityCount() const { return m_Entities.size(); }

            void setMap(std::unique_ptr<Map> map) { m_Map = std::move(map); }
            bool hasMap() const { return m_Map != nullptr; }

            Map* getMap() { return m_Map.get(); }
            const Map* getMap() const { return m_Map.get(); }

            void setInputSystem(std::unique_ptr<InputSystem> inputSystem) { m_InputSystem = std::move(inputSystem); }
            bool hasInputSystem() const { return m_InputSystem != nullptr; }

            InputSystem* getInputSystem() { return m_InputSystem.get(); }
            const InputSystem* getInputSystem() const { return m_InputSystem.get(); }

        protected:
            bool m_Initialized = false;
            std::string m_Name;

            std::vector<std::unique_ptr<Entity>> m_Entities;

            std::unique_ptr<Map> m_Map;
            std::unique_ptr<InputSystem> m_InputSystem;

            void updateEntities(float deltaTime) {
                for (auto& entity : m_Entities) {
                    entity->update(deltaTime);
                }
            }

            void renderEntities() {
                for (auto& entity : m_Entities) {
                    entity->render();
                }
            }
    };

    class SceneManager {
        public:
            static SceneManager& getInstance();

            const bool registerScene(const std::string& name, std::shared_ptr<Scene> scene);
            const bool unregisterScene(const std::string& name);
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

            std::shared_ptr<Scene> m_CurrentScene;
            std::shared_ptr<Scene> m_PreviousScene;

            std::string m_FallbackCurrentSceneText = "No active scene!";

            uint64_t m_LastFrameTime;
    };

    inline SceneManager& getSceneManager() {
        return SceneManager::getInstance();
    }
    
    template<typename T>

    bool createScene(const std::string& name) {
        return getSceneManager().registerScene(name, std::make_shared<T>());
    }
    
    inline bool switchToScene(const std::string& name) {
        return getSceneManager().changeScene(name);
    }
}
