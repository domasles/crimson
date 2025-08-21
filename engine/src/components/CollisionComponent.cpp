#include <components/CollisionComponent.h>
#include <Entity.h>
#include <components/TransformComponent.h>
#include <utils/logger.h>
#include <Scene.h>
#include <cmath>

using namespace engine::utils::logger;

namespace engine {

    void CollisionComponent::init() {
        Component::init();

        m_Collision.type = CollisionType::BLOCK;
        m_Collision.shape = CollisionShape::BOX;
        m_Collision.offset = {0, 0};

        updateFromTransform();
    }

    void CollisionComponent::update(float deltaTime) {
        if (!m_Enabled) return;

        // Always update from Transform - no custom size logic
        updateFromTransform();
        
        updateCollisionEvents();
    }

    void CollisionComponent::updateFromTransform() {
        auto* transform = m_Entity->getComponent<TransformComponent>();
        if (!transform) {
            Logger::engine_warn("CollisionComponent: No Transform component found, using default size");
            m_Collision.size = {32, 32}; // Default fallback size
            return;
        }
        
        // Always use Transform size automatically
        m_Collision.size = transform->getSize();
    }

    bool CollisionComponent::checkCollision(CollisionComponent* other) const {
        if (!m_Enabled || !other->isEnabled()) return false;
        if (m_Collision.type == CollisionType::NONE || other->getCollision().type == CollisionType::NONE) return false;

        // Use unified collision system instead of hardcoded logic
        Vector2 pos1 = getWorldPosition();
        Vector2 pos2 = other->getWorldPosition();
        
        return engine::utils::collision::checkCollision(m_Collision, pos1, other->getCollision(), pos2);
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
        
        auto* transform = m_Entity->getComponent<TransformComponent>();
        if (!transform) {
            return false;
        }
        
        Vector2 worldPos = transform->getPosition();
        Vector2 size = m_Collision.size;
        
        return currentMap->checkCollisionAt(worldPos, size);
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
        
        // Check for entity-to-entity collisions
        if (wouldCollideWithEntitiesAt(newPosition)) {
            return false;
        }
        
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

    bool CollisionComponent::wouldCollideAt(const Vector2& position) const {
        if (!m_Enabled) return false;
        
        Map* currentMap = getCurrentMap();
        if (!currentMap) return false;
        
        // Create entity collision representation at the specified position
        Collision entityCollision{m_Collision.type, m_Collision.shape, m_Collision.offset, m_Collision.size};
        const auto& tiles = currentMap->getCollisionTiles();
        
        for (const auto& tile : tiles) {
            if (engine::utils::collision::checkCollision(entityCollision, position, tile)) {
                return true;
            }
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
        
        // Get all collision components in the scene
        auto collisionComponents = currentScene->getEntitiesWithComponent<CollisionComponent>();
        
        // Create temporary collision at the test position
        Collision testCollision{m_Collision.type, m_Collision.shape, m_Collision.offset, m_Collision.size};
        
        // Check collision with all other entities
        for (auto* otherCollision : collisionComponents) {
            // Skip self
            if (otherCollision == this) continue;
            
            // Skip disabled collision components
            if (!otherCollision->isEnabled()) continue;
            
            // Skip NONE collision types
            if (otherCollision->getCollisionType() == CollisionType::NONE) continue;
            
            // Get other entity's position
            Vector2 otherPosition = otherCollision->getWorldPosition();
            
            // Check collision between this entity at test position and other entity
            if (engine::utils::collision::checkCollision(testCollision, position, 
                                                       otherCollision->getCollision(), otherPosition)) {
                
                // Check if this collision should block movement using BlockCollision rules
                if (otherCollision->getCollisionType() == CollisionType::BLOCK) {
                    if (BlockCollision::shouldBlockMovement()) {
                        return true; // Collision blocks movement
                    }
                }
            }
        }
        
        return false; // No blocking collisions found
    }

}
