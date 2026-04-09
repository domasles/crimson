#pragma once

#include <utils/collision.h>

#include <Component.h>

using namespace engine::utils::collision;

namespace engine {
    class Entity;

    class CollisionComponent : public Component {
        public:
            CollisionComponent() = default;
            virtual ~CollisionComponent() = default;

            void init() override;
            void update(float deltaTime) override;

            void setCollisionType(CollisionType type) { m_Collision.type = type; }
            void setCollisionShape(std::unique_ptr<CollisionShape> shape) { m_Collision.shape = std::move(shape); }
            void setOffset(const Vector2& offset) { m_Collision.offset = offset; }

            bool isEnabled() const { return m_Enabled; }
            void setEnabled(bool enabled) { m_Enabled = enabled; }

            bool getParticipatesInQueries() const { return m_ParticipatesInQueries; }
            void setParticipatesInQueries(bool participates) { m_ParticipatesInQueries = participates; }

            const Collision& getCollision() const { return m_Collision; }
            const CollisionShape* getCollisionShape() const { return m_Collision.shape.get(); }

            CollisionType getCollisionType() const { return m_Collision.type; }

            CollisionResult testCollisionAt(const Vector2& testPosition) const;
            CollisionResult checkCollisionWithEntity(Entity* other) const;
            CollisionResult checkCollisionWithEntityAt(Entity* other, const Vector2& testPosition) const;

            MultiCollisionResult getAllCollisionsAt(const Vector2& testPosition) const;

            Vector2 getWorldPosition() const;
            Vector2 getCollisionWorldPosition() const;

            Vector2 getInterpolatedWorldPosition() const;
            Vector2 getInterpolatedCollisionWorldPosition() const;

        private:
            bool m_Enabled = true;
            bool m_ParticipatesInQueries = false;

            void updateFromTransform();

            Collision m_Collision;
            const std::vector<Entity*>& getOtherCollisionEntities(const Vector2& testPosition) const;
    };
}
