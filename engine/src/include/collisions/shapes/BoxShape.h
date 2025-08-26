#pragma once

#include <collisions/CollisionShape.h>

namespace engine::utils::collision { struct Collision; }

using namespace engine::utils::collision;

namespace engine::collisions::shapes {
    class BoxShape : public CollisionShape {
        public:
            CollisionResult checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const override;
            CollisionResult checkCollisionWithBox(const Vector2& myPos, const Vector2& mySize, const Vector2& boxPos, const Vector2& boxSize) const override;
            
            std::unique_ptr<CollisionShape> clone() const override { return std::make_unique<BoxShape>(*this); }
    };
}
