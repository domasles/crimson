#pragma once

#include <Component.h>
#include <utils/collision.h>
#include <functional>
#include <vector>

using namespace engine::utils::collision;

namespace engine {
    class Entity;

    class CollisionComponent : public Component {
        public:
            CollisionComponent() = default;
            virtual ~CollisionComponent() = default;

            void init() override;
            void update(float deltaTime) override;

            // Configuration - automatically uses Transform component size
            void setCollisionType(CollisionType type) { m_Collision.type = type; }
            void setCollisionShape(CollisionShape shape) { m_Collision.shape = shape; }
            void setOffset(const Vector2& offset) { m_Collision.offset = offset; }
            
            // Getters
            bool isEnabled() const { return m_Enabled; }
            const Collision& getCollision() const { return m_Collision; }
            CollisionType getCollisionType() const { return m_Collision.type; }
            CollisionShape getCollisionShape() const { return m_Collision.shape; }

            // Simple configuration
            void setEnabled(bool enabled) { m_Enabled = enabled; }
            
            // Collision detection
            bool checkCollision(CollisionComponent* other) const;
            bool checkWorldCollision() const;  // Check collision with map
            bool canMoveTo(const Vector2& newPosition) const;  // Check if movement is allowed
            bool wouldCollideAt(const Vector2& position) const;  // Check collision without moving
            Vector2 getWorldPosition() const;
            Vector2 getCollisionWorldPosition() const;

            // Event callbacks
            using CollisionCallback = std::function<void(Entity*)>;
            void setOnCollisionEnter(CollisionCallback callback) { m_OnCollisionEnter = callback; }

        private:
            Collision m_Collision;
            bool m_Enabled = true;

            // Event callbacks
            CollisionCallback m_OnCollisionEnter;

            // Collision state tracking
            std::vector<Entity*> m_CollidingEntities;

            void updateFromTransform();
            void updateCollisionEvents();
            
            // Get map from current scene via SceneManager
            class Map* getCurrentMap() const;
            
            // Check collision with other entities at position
            bool wouldCollideWithEntitiesAt(const Vector2& position) const;
    };
}
