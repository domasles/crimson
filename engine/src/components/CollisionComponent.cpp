#include <components/CollisionComponent.h>
#include <Entity.h>
#include <components/TransformComponent.h>
#include <utils/logger.h>
#include <Map.h>
#include <cmath>

using namespace engine::utils::logger;

namespace engine {
    Map* CollisionComponent::s_WorldMap = nullptr;

    void CollisionComponent::init() {
        Component::init();

        m_Collision.type = CollisionType::BLOCK;
        m_Collision.shape = CollisionShape::BOX;
        m_Collision.offset = {0, 0};

        updateFromTransform();
    }

    void CollisionComponent::update(float deltaTime) {
        if (!m_Enabled) return;

        if (!m_UseCustomSize) {
            updateFromTransform();
        }
        
        updateCollisionEvents();
    }

    void CollisionComponent::updateFromTransform() {
        auto* transform = m_Entity->getComponent<TransformComponent>();
        if (!transform) {
            Logger::engine_warn("CollisionComponent: No Transform component found, using default size");
            if (!m_UseCustomSize) {
                m_Collision.size = {32, 32}; // Default fallback size
            }
            return;
        }
        
        // Automatically use Transform size if no custom size is set
        if (!m_UseCustomSize) {
            m_Collision.size = transform->getSize();
        }
    }

    bool CollisionComponent::checkCollision(CollisionComponent* other) const {
        if (!m_Enabled || !other->isEnabled()) return false;
        if (m_Collision.type == CollisionType::NONE || other->getCollision().type == CollisionType::NONE) return false;

        // For now, only support box collision as requested
        if (m_Collision.shape == CollisionShape::BOX && other->getCollision().shape == CollisionShape::BOX) {
            return checkBoxCollision(other);
        }

        return false;
    }

    bool CollisionComponent::checkBoxCollision(CollisionComponent* other) const {
        Vector2 pos1 = getCollisionWorldPosition();
        Vector2 size1 = m_Collision.size;
        
        Vector2 pos2 = other->getCollisionWorldPosition();
        Vector2 size2 = other->getCollision().size;

        // AABB collision detection
        return (pos1.getRawX() < pos2.getRawX() + size2.getRawX() &&
                pos1.getRawX() + size1.getRawX() > pos2.getRawX() &&
                pos1.getRawY() < pos2.getRawY() + size2.getRawY() &&
                pos1.getRawY() + size1.getRawY() > pos2.getRawY());
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
        if (!m_Enabled || !s_WorldMap) return false;
        
        auto* transform = m_Entity->getComponent<TransformComponent>();
        if (!transform) {
            return false;
        }
        
        Vector2 worldPos = transform->getPosition();
        Vector2 size = m_Collision.size;
        
        return s_WorldMap->checkCollisionAt(worldPos, size);
    }

    bool CollisionComponent::canMoveTo(const Vector2& newPosition) const {
        if (!m_Enabled) return true; // Allow movement if collision disabled
        
        auto* transform = m_Entity->getComponent<TransformComponent>();
        if (!transform) {
            return true; // Allow movement if no transform
        }
        
        // Store current position
        Vector2 originalPos = transform->getPosition();
        
        // Temporarily set new position to check collision
        // We need to directly access the transform to avoid recursive calls
        Transform& directTransform = const_cast<TransformComponent*>(transform)->getTransform();
        directTransform.setPosition(newPosition);
        
        // Check for world collision at new position
        bool wouldCollide = checkWorldCollision();
        
        // Restore original position
        directTransform.setPosition(originalPos);
        
        if (wouldCollide) {
            return false;
        }
        
        // TODO: Check for entity-to-entity collisions here if needed
        
        return true; // Movement allowed
    }

    void CollisionComponent::updateCollisionEvents() {
        // Simple collision event tracking - for now just check collision enter
        // More sophisticated tracking can be added later if needed
        
        if (m_OnCollisionEnter) {
            // This would be called by the collision system when collisions are detected
            // For now, just a placeholder for the event system
        }
    }

}
