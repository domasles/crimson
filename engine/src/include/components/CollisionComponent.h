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

            // Simple configuration - automatically uses Transform component
            void setEnabled(bool enabled) { m_Enabled = enabled; }
            
            // Optional overrides (if you don't want automatic Transform integration)
            void setCollisionType(CollisionType type) { m_Collision.type = type; }
            void setCollisionShape(CollisionShape shape) { m_Collision.shape = shape; }
            void setCustomSize(const Vector2& size) { m_Collision.size = size; m_UseCustomSize = true; }
            void setOffset(const Vector2& offset) { m_Collision.offset = offset; }
            
            // Advanced configuration (for future features)
            void setCollision(const Collision& collision) { m_Collision = collision; m_UseCustomSize = true; }
            
            // Getters
            bool isEnabled() const { return m_Enabled; }
            const Collision& getCollision() const { return m_Collision; }
            CollisionType getCollisionType() const { return m_Collision.type; }
            CollisionShape getCollisionShape() const { return m_Collision.shape; }

            // Collision detection
            bool checkCollision(CollisionComponent* other) const;
            bool checkWorldCollision() const;  // Check collision with map
            bool canMoveTo(const Vector2& newPosition) const;  // Check if movement is allowed
            Vector2 getWorldPosition() const;
            Vector2 getCollisionWorldPosition() const;

            // Event callbacks
            using CollisionCallback = std::function<void(Entity*)>;
            void setOnCollisionEnter(CollisionCallback callback) { m_OnCollisionEnter = callback; }

            // Static map access for world collision
            static void setWorldMap(class Map* map) { s_WorldMap = map; }

        private:
            Collision m_Collision;
            bool m_Enabled = true;
            bool m_UseCustomSize = false;  // Whether to use custom size or Transform size

            // Event callbacks
            CollisionCallback m_OnCollisionEnter;

            // Collision state tracking
            std::vector<Entity*> m_CollidingEntities;

            void updateFromTransform();
            bool checkBoxCollision(CollisionComponent* other) const;
            void updateCollisionEvents();

            // Static world map for collision checking
            static class Map* s_WorldMap;
    };
}
