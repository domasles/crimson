#include <pch.h>

#include <utils/logger.h>

#include <components/CollisionComponent.h>
#include <components/TransformComponent.h>

#include <Entity.h>
#include <Scene.h>

using namespace engine::utils::logger;

namespace engine {

    void CollisionComponent::init() {
        Component::init();

        m_Collision.type = std::make_unique<BlockCollision>();
        m_Collision.shape = std::make_unique<BoxShape>();
        m_Collision.offset = {0, 0};

        updateFromTransform();
    }

    void CollisionComponent::update(float deltaTime) {
        if (!m_Enabled) return;

        updateFromTransform();
    }

    void CollisionComponent::updateFromTransform() {
        auto* transform = m_Entity->getComponent<TransformComponent>();
        if (!transform) {
            Logger::engine_warn("CollisionComponent: No Transform component found, using default size");
            m_Collision.size = {32, 32};
            return;
        }

        m_Collision.size = transform->getSize();
    }

    bool CollisionComponent::checkCollision(CollisionComponent* other) const {
        if (!m_Enabled || !other->isEnabled()) return false;
        if (!m_Collision.type->shouldBlock() || !other->getCollision().type->shouldBlock()) return false;

        Vector2 pos1 = getWorldPosition();
        Vector2 pos2 = other->getWorldPosition();

        return m_Collision.shape->checkCollisionBetween(m_Collision, pos1, other->getCollision(), pos2);
    }

    Vector2 CollisionComponent::getWorldPosition() const {
        if (auto transform = m_Entity->getComponent<TransformComponent>()) {
            return transform->getPosition();
        }

        return {0, 0};
    }

    Vector2 CollisionComponent::getCollisionWorldPosition() const {
        Vector2 worldPos = getWorldPosition();
        return {worldPos.getRawX() + m_Collision.offset.getRawX(), worldPos.getRawY() + m_Collision.offset.getRawY()};
    }

    bool CollisionComponent::checkWorldCollision() const {
        if (!m_Enabled) return false;

        Map* currentMap = getCurrentMap();
        if (!currentMap) return false;

        Vector2 worldPos = getWorldPosition();
        return currentMap->checkCollisionAt(m_Collision, worldPos);
    }
    
    bool CollisionComponent::canMoveTo(const Vector2& newPosition) const {
        if (!m_Enabled) return true;

        auto* transform = m_Entity->getComponent<TransformComponent>();
        if (!transform) return true;

        Vector2 originalPos = transform->getPosition();
        Transform& directTransform = const_cast<TransformComponent*>(transform)->getTransform();

        directTransform.setPosition(newPosition);
        bool wouldCollide = checkWorldCollision();
        directTransform.setPosition(originalPos);

        if (wouldCollide) return false;
        if (wouldCollideWithEntitiesAt(newPosition)) return false;

        return true;
    }

    bool CollisionComponent::wouldCollideAt(const Vector2& position) const {
        if (!m_Enabled) return false;

        Map* currentMap = getCurrentMap();
        if (!currentMap) return false;

        Vector2 testWorldPos = {
            position.getRawX() + m_Collision.offset.getRawX(),
            position.getRawY() + m_Collision.offset.getRawY()
        };

        const auto& tiles = currentMap->getCollisionTiles();

        for (const auto& tile : tiles) {
            if (m_Collision.shape->checkCollisionWithTile(testWorldPos, m_Collision.size, tile)) return true;
        }

        return false;
    }

    Map* CollisionComponent::getCurrentMap() const {
        auto& sceneManager = getSceneManager();
        auto currentScene = sceneManager.getCurrentScene();

        if (currentScene && currentScene->hasMap()) {
            return currentScene->getMap();
        }

        return nullptr;
    }

    bool CollisionComponent::wouldCollideWithEntitiesAt(const Vector2& position) const {
        if (!m_Enabled) return false;
        
        auto& sceneManager = getSceneManager();
        auto currentScene = sceneManager.getCurrentScene();

        if (!currentScene) return false;
        auto collisionComponents = currentScene->getEntitiesWithComponent<CollisionComponent>();

        for (auto* otherCollision : collisionComponents) {
            if (otherCollision == this) continue;
            if (!otherCollision->isEnabled()) continue;
            if (!otherCollision->getCollisionType()->shouldBlock()) continue;

            Vector2 otherPosition = otherCollision->getWorldPosition();

            Vector2 testWorldPos = {
                position.getRawX() + m_Collision.offset.getRawX(),
                position.getRawY() + m_Collision.offset.getRawY()
            };

            Vector2 otherWorldPos = {
                otherPosition.getRawX() + otherCollision->getCollision().offset.getRawX(),
                otherPosition.getRawY() + otherCollision->getCollision().offset.getRawY()
            };

            if (m_Collision.shape->checkCollision(testWorldPos, m_Collision.size, *otherCollision->getCollision().shape, otherWorldPos, otherCollision->getCollision().size)) {
                if (otherCollision->getCollisionType()->shouldBlock()) return true;
            }
        }
        
        return false;
    }

}
