#include <pch.h>

#include <utils/collision.h>

#include <collisions/shapes/BoxShape.h>

namespace engine::collisions::shapes {
    bool BoxShape::checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const {
        return other.checkCollisionWithBox(otherPos, otherSize, myPos, mySize);
    }

    bool BoxShape::checkCollisionWithBox(const Vector2& myPos, const Vector2& mySize, const Vector2& boxPos, const Vector2& boxSize) const {
        return (myPos.getRawX() < boxPos.getRawX() + boxSize.getRawX() &&
                myPos.getRawX() + mySize.getRawX() > boxPos.getRawX() &&
                myPos.getRawY() < boxPos.getRawY() + boxSize.getRawY() &&
                myPos.getRawY() + mySize.getRawY() > boxPos.getRawY());
    }

    bool BoxShape::checkCollisionWithTile(const Vector2& myPos, const Vector2& mySize, const CollisionTile& tile) const {
        if (!tile.type->shouldBlock()) return false;
        return tile.shape->checkCollisionWithBox(tile.worldPosition, tile.size, myPos, mySize);
    }

    bool BoxShape::checkCollisionBetween(const Collision& collision1, const Vector2& pos1, const Collision& collision2, const Vector2& pos2) const {
        if (!collision1.type->shouldBlock() || !collision2.type->shouldBlock()) return false;

        Vector2 worldPos1 = {
            pos1.getRawX() + collision1.offset.getRawX(),
            pos1.getRawY() + collision1.offset.getRawY()
        };

        Vector2 worldPos2 = {
            pos2.getRawX() + collision2.offset.getRawX(),
            pos2.getRawY() + collision2.offset.getRawY()
        };

        return collision1.shape->checkCollision(worldPos1, collision1.size, *collision2.shape, worldPos2, collision2.size);
    }
}
