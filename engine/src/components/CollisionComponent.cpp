#include <pch.h>

#include <utils/logger.h>
#include <utils/gjk.h>

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

        const auto& otherEntities = getOtherCollisionEntities(testPosition);

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

        auto collisionResult = engine::utils::gjk::test(
            *m_Collision.shape, myWorldPos, m_Collision.size,
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

        const auto& otherEntities = getOtherCollisionEntities(testPosition);

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

    Vector2 CollisionComponent::getInterpolatedWorldPosition() const {
        if (auto transform = m_Entity->getComponent<TransformComponent>()) {
            return transform->getInterpolatedPosition();
        }

        return {0, 0};
    }

    Vector2 CollisionComponent::getInterpolatedCollisionWorldPosition() const {
        Vector2 worldPos = getInterpolatedWorldPosition();
        return { worldPos.getRawX() + m_Collision.offset.getRawX(), worldPos.getRawY() + m_Collision.offset.getRawY() };
    }

    const std::vector<Entity*>& CollisionComponent::getOtherCollisionEntities(const Vector2& testPosition) const {
        thread_local static std::vector<Entity*> s_otherEntities;
        s_otherEntities.clear();

        Scene* currentScene = getSceneManager().getCurrentSceneRaw();

        if (!currentScene) return s_otherEntities;

        Vector2 worldPos = {
            testPosition.getRawX() + m_Collision.offset.getRawX(),
            testPosition.getRawY() + m_Collision.offset.getRawY()
        };

        AABB queryAABB = m_Collision.shape->getBoundingBox(worldPos, m_Collision.size);

        thread_local static std::vector<CollisionComponent*> s_candidates;

        s_candidates.clear();
        currentScene->getBVH().query(queryAABB, s_candidates);

        s_otherEntities.reserve(s_candidates.size());

        for (auto* candidate : s_candidates) {
            if (candidate == this || !candidate->isEnabled()) continue;

            if (candidate->getParticipatesInQueries() || (candidate->getCollisionType() && candidate->getCollisionType()->shouldBlock())) {
                s_otherEntities.push_back(candidate->getEntity());
            }
        }

        return s_otherEntities;
    }
}
