#pragma once

#include <collisions/BVH.h>
#include <ui/UIContext.h>

#include <Entity.h>
#include <Input.h>
#include <Core.h>
#include <Map.h>

using namespace engine::collisions;
using namespace engine::ui;

namespace engine {
    class Scene {
        public:
            virtual ~Scene() = default;

            virtual void init() = 0;
            virtual void update(float deltaTime) = 0;
            virtual void render() = 0;

            virtual void onEnter(bool resumed) {}
            virtual void onExit() {}
            virtual void reset();

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

            void updateUI()  { m_UIContext.update(); }
            void renderUI()  { m_UIContext.render(); }

            bool isUICapturingInput() const { return m_UIContext.isCapturingInput(); }
            bool processUIEvent(const SDL_Event& event) { return m_UIContext.processEvent(event); }

            void setMap(std::unique_ptr<Map> map) { m_Map = std::move(map); }
            bool hasMap() const { return m_Map != nullptr; }

            void setInputSystem(std::unique_ptr<InputSystem> inputSystem) { m_InputSystem = std::move(inputSystem); }
            bool hasInputSystem() const { return m_InputSystem != nullptr; }

            void  setBackgroundColor(const Color& color) { m_BackgroundColor = color; }
            void  setOutOfBoundsColor(const Color& color) { m_OutOfBoundsColor = color; }

            void rebuildBVH();

            Map* getMap() { return m_Map.get(); }
            const Map* getMap() const { return m_Map.get(); }

            InputSystem* getInputSystem() { return m_InputSystem.get(); }
            const InputSystem* getInputSystem() const { return m_InputSystem.get(); }

            BVH& getBVH() { return m_BVH; }
            const BVH& getBVH() const { return m_BVH; }

            UIContext& getUIContext() { return m_UIContext; }

            Color getBackgroundColor()  const { return m_BackgroundColor; }
            Color getOutOfBoundsColor() const { return m_OutOfBoundsColor; }

        protected:
            bool m_Initialized = false;
            bool m_CollisionCacheDirty = true;

            std::string m_Name;
            std::vector<std::unique_ptr<Entity>> m_Entities;

            std::unique_ptr<Map> m_Map;
            std::unique_ptr<InputSystem> m_InputSystem;

            BVH m_BVH;
            std::vector<CollisionComponent*> m_CollisionComponents;

            UIContext m_UIContext;

            Color m_BackgroundColor  { 0.0f, 0.0f, 0.0f, 1.0f };
            Color m_OutOfBoundsColor { 0.0f, 0.0f, 0.0f, 1.0f };

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

            bool registerScene(const std::string& name, std::shared_ptr<Scene> scene);
            bool unregisterScene(const std::string& name);
            bool changeScene(const std::string& name);

            bool update();
            bool render();
            void updateUI();
            void renderUI();
            void prepareRender();

            bool processUIEvent(const SDL_Event& event);

            Color getBackgroundColor() const { return m_CurrentScene ? m_CurrentScene->getBackgroundColor() : Color{0.0f, 0.0f, 0.0f, 1.0f}; }
            Color getOutOfBoundsColor() const { return m_CurrentScene ? m_CurrentScene->getOutOfBoundsColor() : Color{0.0f, 0.0f, 0.0f, 1.0f}; }

            const std::string& getCurrentSceneName() const;
            std::shared_ptr<Scene> getCurrentScene() const;

            Scene* getCurrentSceneRaw() const;

        private:
            SceneManager() : m_LastFrameTime(SDL_GetTicksNS()) {}
            ~SceneManager() = default;

            SceneManager(const SceneManager&) = delete;
            SceneManager& operator=(const SceneManager&) = delete;

            void applyPendingSwitch();

            std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;

            std::shared_ptr<Scene> m_CurrentScene;
            std::shared_ptr<Scene> m_PreviousScene;

            std::string m_FallbackCurrentSceneText = "No active scene!";
            std::string m_PendingSceneName;

            uint64_t m_LastFrameTime;
            float m_PhysicsAccumulator = 0.0f;

            static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
            static constexpr float MAX_DELTA_TIME = 0.25f;
    };

    inline SceneManager& getSceneManager() { return SceneManager::getInstance(); }
    inline bool switchToScene(const std::string& name) { return getSceneManager().changeScene(name); }

    inline void setBackgroundColor(const Color& color) { if (auto* s = SceneManager::getInstance().getCurrentSceneRaw()) s->setBackgroundColor(color); }
    inline void setOutOfBoundsColor(const Color& color) { if (auto* s = SceneManager::getInstance().getCurrentSceneRaw()) s->setOutOfBoundsColor(color); }

    template<typename T>

    bool createScene(const std::string& name) {
        static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");
        return getSceneManager().registerScene(name, std::make_shared<T>());
    }

}
