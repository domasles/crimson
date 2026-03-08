#include <pch.h>

#include <utils/logger.h>

#include <components/CollisionComponent.h>

#include <Scene.h>

using namespace engine::utils::logger;
using namespace engine::utils::math;

namespace engine {
    void Scene::prepareRender(float alpha) {
        for (auto& entity : m_Entities) {
            if (auto* transform = entity->getComponent<TransformComponent>()) {
                Vector2 prevPos = transform->getPreviousPosition();
                Vector2 currentPos = transform->getPosition();
                Vector2 interpolatedPos = prevPos + (currentPos - prevPos) * alpha;

                transform->setInterpolatedPosition(interpolatedPos);

                float prevRot = transform->getPreviousRotation();
                float currentRot = transform->getRotation();
                float interpolatedRot = Geometry::lerpAngle(prevRot, currentRot, alpha);

                transform->setInterpolatedRotation(interpolatedRot);

                Vector2 prevSize = transform->getPreviousSize();
                Vector2 currentSize = transform->getSize();
                Vector2 interpolatedSize = prevSize + (currentSize - prevSize) * alpha;

                transform->setInterpolatedSize(interpolatedSize);

                Vector2 prevScale = transform->getPreviousScale();
                Vector2 currentScale = transform->getScale();
                Vector2 interpolatedScale = prevScale + (currentScale - prevScale) * alpha;

                transform->setInterpolatedScale(interpolatedScale);
            }
        }
    }

    void Scene::removeEntity(Entity* entity) {
        m_CollisionCacheDirty = true;
        m_Entities.erase(std::remove_if(m_Entities.begin(), m_Entities.end(), [entity](const auto& ptr) { return ptr.get() == entity; }), m_Entities.end());
    }

    void Scene::reset() {
        m_Entities.clear();
        m_BVH.clear();
        m_CollisionComponents.clear();
        m_CollisionCacheDirty = true;
        m_Initialized = false;
    }

    void Scene::rebuildBVH() {
        if (m_CollisionCacheDirty) {
            m_CollisionComponents.clear();

            for (auto& entity : m_Entities) {
                if (auto* comp = entity->getComponent<CollisionComponent>()) {
                    m_CollisionComponents.push_back(comp);
                }
            }

            m_CollisionCacheDirty = false;
        }

        thread_local static std::vector<CollisionComponent*> s_enabled;

        s_enabled.clear();
        s_enabled.reserve(m_CollisionComponents.size());

        for (auto* comp : m_CollisionComponents) {
            if (comp->isEnabled()) s_enabled.push_back(comp);
        }

        m_BVH.update(s_enabled);
    }

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

    bool SceneManager::registerScene(const std::string& name, std::shared_ptr<Scene> scene) {
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

    bool SceneManager::unregisterScene(const std::string& name) {
        auto it = m_Scenes.find(name);

        if (it == m_Scenes.end()) {
            Logger::engine_warn("Scene {} not found. Cannot unregister!", name);
            return false;
        }

        m_Scenes.erase(it);
        return true;
    }

    bool SceneManager::changeScene(const std::string& name) {
        if (m_Scenes.find(name) == m_Scenes.end()) {
            Logger::engine_error("Scene {} not found!", name);
            return false;
        }

        m_PendingSceneName = name;
        return true;
    }

    void SceneManager::applyPendingSwitch() {
        auto newScene = m_Scenes.at(m_PendingSceneName);

        if (m_CurrentScene) {
            m_CurrentScene->onExit();
        }

        m_PreviousScene = m_CurrentScene;
        m_CurrentScene = newScene;

        bool resumed = newScene->getInitialized();

        if (!resumed) {
            newScene->setInitialized(true);
            newScene->init();

            newScene->rebuildBVH();
            newScene->update(0.0f);
        }

        newScene->onEnter(resumed);
    }

    bool SceneManager::update() {
        uint64_t currentTime = SDL_GetTicksNS();
        float rawDeltaTime = (currentTime - m_LastFrameTime) / 1'000'000'000.0f;
        m_LastFrameTime = currentTime;

        if (!m_PendingSceneName.empty()) {
            applyPendingSwitch();

            m_PendingSceneName.clear();
            m_PhysicsAccumulator = 0;
            m_LastFrameTime = SDL_GetTicksNS();

            rawDeltaTime = 0;
        }

        if (!m_CurrentScene) {
            Logger::engine_error("No current scene is found or selected!");
            return false;
        }

        float cappedDeltaTime = std::min(rawDeltaTime, MAX_DELTA_TIME);
        m_PhysicsAccumulator += cappedDeltaTime;

        while (m_PhysicsAccumulator >= FIXED_TIMESTEP) {
            m_CurrentScene->rebuildBVH();
            m_CurrentScene->update(FIXED_TIMESTEP);

            m_PhysicsAccumulator -= FIXED_TIMESTEP;
        }
        
        return true;
    }

    bool SceneManager::render() {
        if (!m_CurrentScene) {
            Logger::engine_error("No current scene is found or selected!");
            return false;
        }

        m_CurrentScene->render();
        return true;
    }

    void SceneManager::updateUI() {
        if (m_CurrentScene) m_CurrentScene->updateUI();
    }

    void SceneManager::renderUI() {
        if (m_CurrentScene) m_CurrentScene->renderUI();
    }

    void SceneManager::resizeUI(int width, int height, float dpRatio) {
        if (m_CurrentScene) m_CurrentScene->getUIContext().resize(width, height, dpRatio);
    }

    void SceneManager::prepareRender() {
        if (m_CurrentScene) {
            float alpha = m_PhysicsAccumulator / FIXED_TIMESTEP;
            m_CurrentScene->prepareRender(alpha);
        }
    }

    bool SceneManager::processUIEvent(const SDL_Event& event) {
        if (m_CurrentScene) return m_CurrentScene->processUIEvent(event);
        return false;
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

    Scene* SceneManager::getCurrentSceneRaw() const {
        return m_CurrentScene.get();
    }
}
