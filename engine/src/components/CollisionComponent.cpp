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

    CollisionResult CollisionComponent::testCollisionAt(const Vector2& testPosition) const {
        if (!m_Enabled) return CollisionResult{};

        auto otherEntities = getOtherCollisionEntities();
        for (auto* otherEntity : otherEntities) {
            auto* otherCollision = otherEntity->getComponent<CollisionComponent>();
            if (!otherCollision || !otherCollision->isEnabled()) continue;

            CollisionResult result = checkCollisionWithEntityAt(otherEntity, testPosition);
            if (result.hasCollision) return result;
        }

        return CollisionResult{};
    }

    CollisionResult CollisionComponent::checkCollisionWithEntity(Entity* other) const {
        return checkCollisionWithEntityAt(other, getWorldPosition());
    }

    CollisionResult CollisionComponent::checkCollisionWithEntityAt(Entity* other, const Vector2& testPosition) const {
        if (!m_Enabled || !other) return CollisionResult{};

        auto* otherCollision = other->getComponent<CollisionComponent>();
        if (!otherCollision || !otherCollision->isEnabled()) return CollisionResult{};

        Vector2 myWorldPos = {
            testPosition.getRawX() + m_Collision.offset.getRawX(),
            testPosition.getRawY() + m_Collision.offset.getRawY()
        };

        Vector2 otherWorldPos = otherCollision->getCollisionWorldPosition();

        auto collisionResult = m_Collision.shape->checkCollision(
            myWorldPos, m_Collision.size,
            *otherCollision->getCollision().shape,
            otherWorldPos, otherCollision->getCollision().size
        );

        CollisionResult result;

        result.hasCollision = collisionResult.hasCollision;
        result.hitType = collisionResult.hasCollision ? otherCollision->getCollisionType() : nullptr;
        result.hitEntity = collisionResult.hasCollision ? other : nullptr;
        result.contactPoint = collisionResult.contactPoint;
        result.contactNormal = collisionResult.contactNormal;

        return result;
    }

    MultiCollisionResult CollisionComponent::getAllCollisionsAt(const Vector2& testPosition) const {
        MultiCollisionResult result;
        
        if (!m_Enabled) return result;
        auto otherEntities = getOtherCollisionEntities();

        for (auto* otherEntity : otherEntities) {
            CollisionResult entityCollision = checkCollisionWithEntityAt(otherEntity, testPosition);
            if (entityCollision.hasCollision) {
                result.collisions.push_back(entityCollision);
            }
        }

        return result;
    }

    Vector2 CollisionComponent::getWorldPosition() const {
        if (auto transform = m_Entity->getComponent<TransformComponent>()) return transform->getPosition();

        return {0, 0};
    }

    Vector2 CollisionComponent::getCollisionWorldPosition() const {
        Vector2 worldPos = getWorldPosition();
        return { worldPos.getRawX() + m_Collision.offset.getRawX(), worldPos.getRawY() + m_Collision.offset.getRawY() };
    }

    std::vector<Entity*> CollisionComponent::getOtherCollisionEntities() const {
        auto& sceneManager = getSceneManager();
        auto currentScene = sceneManager.getCurrentScene();

        if (!currentScene) return {};

        auto collisionComponents = currentScene->getEntitiesWithComponent<CollisionComponent>();
        std::vector<Entity*> otherEntities;

        for (auto* otherCollision : collisionComponents) {
            if (otherCollision == this) continue;
            if (!otherCollision->isEnabled()) continue;

            if (otherCollision->getParticipatesInQueries() || (otherCollision->getCollisionType() && otherCollision->getCollisionType()->shouldBlock())) {
                otherEntities.push_back(otherCollision->getEntity());
            }
        }

        return otherEntities;
    }
}
