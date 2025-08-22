#pragma once

#include <collisions/CollisionShape.h>

namespace engine::utils::collision {
    struct Collision;
    struct CollisionTile;
}

using namespace engine::utils::collision;

namespace engine::collisions::shapes {
    class BoxShape : public CollisionShape {
        public:
            bool checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const override;
            bool checkCollisionWithBox(const Vector2& myPos, const Vector2& mySize, const Vector2& boxPos, const Vector2& boxSize) const override;
            bool checkCollisionWithTile(const Vector2& myPos, const Vector2& mySize, const CollisionTile& tile) const override;
            bool checkCollisionBetween(const Collision& collision1, const Vector2& pos1, const Collision& collision2, const Vector2& pos2) const override;
            
            std::unique_ptr<CollisionShape> clone() const override { return std::make_unique<BoxShape>(*this); }
    };
}
