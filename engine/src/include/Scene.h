#pragma once

#include <Entity.h>
#include <Input.h>
#include <Core.h>
#include <Map.h>

#include <collisions/BVH.h>

using namespace engine::collisions;

namespace engine {
    class Scene {
        public:
            virtual ~Scene() = default;

            virtual void init() = 0;
            virtual void update(float deltaTime) = 0;
            virtual void render() = 0;

            virtual void prepareRender(float alpha);

            void setInitialized(bool value) { m_Initialized = value; }

            bool getInitialized() const { return m_Initialized; }
            const std::string& getName() const { return m_Name; }

            template<typename T, typename... Args>

            T* createEntity(Args&&... args) {
                auto entity = std::make_unique<T>(std::forward<Args>(args)...);
                T* ptr = entity.get();

                m_Entities.push_back(std::move(entity));
                m_CollisionCacheDirty = true;

                return ptr;
            }

            void removeEntity(Entity* entity);

            template<typename T>

            T* findEntity() {
                static_assert(std::is_base_of_v<Entity, T>, "T must derive from Entity");

                for (auto& entity : m_Entities) {
                    if (auto* typed = dynamic_cast<T*>(entity.get())) {
                        return typed;
                    }
                }

                return nullptr;
            }

            size_t getEntityCount() const { return m_Entities.size(); }
            const std::vector<std::unique_ptr<Entity>>& getEntities() const { return m_Entities; }
            
            template<typename T>

            std::vector<T*> getEntitiesWithComponent() {
                static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
                std::vector<T*> result;

                for (auto& entity : m_Entities) {
                    if (auto* component = entity->getComponent<T>()) {
                        result.push_back(component);
                    }
                }

                return result;
            }

            void setMap(std::unique_ptr<Map> map) { m_Map = std::move(map); }
            bool hasMap() const { return m_Map != nullptr; }

            Map* getMap() { return m_Map.get(); }
            const Map* getMap() const { return m_Map.get(); }

            void setInputSystem(std::unique_ptr<InputSystem> inputSystem) { m_InputSystem = std::move(inputSystem); }
            bool hasInputSystem() const { return m_InputSystem != nullptr; }

            InputSystem* getInputSystem() { return m_InputSystem.get(); }
            const InputSystem* getInputSystem() const { return m_InputSystem.get(); }

            void rebuildBVH();

            BVH& getBVH() { return m_BVH; }
            const BVH& getBVH() const { return m_BVH; }

        protected:
            bool m_Initialized = false;
            bool m_CollisionCacheDirty = true;

            std::string m_Name;
            std::vector<std::unique_ptr<Entity>> m_Entities;

            std::unique_ptr<Map> m_Map;
            std::unique_ptr<InputSystem> m_InputSystem;

            BVH m_BVH;
            std::vector<CollisionComponent*> m_CollisionComponents;

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

            // Prepare entities for rendering with interpolation
            void prepareRender() {
                if (m_CurrentScene) {
                    float alpha = m_PhysicsAccumulator / FIXED_TIMESTEP;
                    m_CurrentScene->prepareRender(alpha);
                }
            }

            const std::string& getCurrentSceneName() const;
            std::shared_ptr<Scene> getCurrentScene() const;
            Scene* getCurrentSceneRaw() const;

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
            float m_PhysicsAccumulator = 0.0f;

            static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
            static constexpr float MAX_DELTA_TIME = 0.25f;
    };

    inline SceneManager& getSceneManager() {
        return SceneManager::getInstance();
    }
    
    template<typename T>

    bool createScene(const std::string& name) {
        static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");
        return getSceneManager().registerScene(name, std::make_shared<T>());
    }
    
    inline bool switchToScene(const std::string& name) {
        return getSceneManager().changeScene(name);
    }
}
