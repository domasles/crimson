#pragma once

#include <utils/collision.h>

#include <collisions/types/TriggerCollision.h>
#include <collisions/types/BlockCollision.h>
#include <collisions/types/NoneCollision.h>
#include <collisions/shapes/BoxShape.h>

#include <Component.h>

using namespace engine::utils::collision;
using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

namespace engine {
    class Entity;

    class CollisionComponent : public Component {
        public:
            CollisionComponent() = default;
            virtual ~CollisionComponent() = default;

            void init() override;
            void update(float deltaTime) override;

            void setCollisionType(std::unique_ptr<CollisionType> type) { m_Collision.type = std::move(type); }
            void setCollisionShape(std::unique_ptr<CollisionShape> shape) { m_Collision.shape = std::move(shape); }
            void setOffset(const Vector2& offset) { m_Collision.offset = offset; }

            bool isEnabled() const { return m_Enabled; }
            void setEnabled(bool enabled) { m_Enabled = enabled; }

            bool getParticipatesInQueries() const { return m_ParticipatesInQueries; }
            void setParticipatesInQueries(bool participates) { m_ParticipatesInQueries = participates; }

            const Collision& getCollision() const { return m_Collision; }
            const CollisionType* getCollisionType() const { return m_Collision.type.get(); }
            const CollisionShape* getCollisionShape() const { return m_Collision.shape.get(); }

            // New pure collision detection methods
            CollisionResult testCollisionAt(const Vector2& testPosition) const;
            CollisionResult checkCollisionWithEntity(Entity* other) const;
            CollisionResult checkCollisionWithEntityAt(Entity* other, const Vector2& testPosition) const;
            MultiCollisionResult getAllCollisionsAt(const Vector2& testPosition) const;

            Vector2 getWorldPosition() const;
            Vector2 getCollisionWorldPosition() const;

        private:
            Collision m_Collision;
            bool m_Enabled = true;
            bool m_ParticipatesInQueries = false; // Explicit opt-in for scene queries

            void updateFromTransform();
            class Map* getCurrentMap() const;
            bool testMapCollisionAt(const Vector2& testPosition) const;
            std::vector<Entity*> getOtherCollisionEntities() const;
    };
}
