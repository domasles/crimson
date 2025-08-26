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
}
