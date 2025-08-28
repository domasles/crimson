#include <pch.h>

#include <utils/collision.h>

#include <collisions/shapes/BoxShape.h>

namespace engine::collisions::shapes {
    CollisionResult BoxShape::checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const {
        auto result = other.checkCollisionWithBox(otherPos, otherSize, myPos, mySize);

        if (result.hasCollision) {
            result.contactNormal = Vector2{ -result.contactNormal.getRawX(), -result.contactNormal.getRawY() };
        }

        return result;
    }

    CollisionResult BoxShape::checkCollisionWithBox(const Vector2& myPos, const Vector2& mySize, const Vector2& boxPos, const Vector2& boxSize) const {
        CollisionResult result;

        bool hasCollision = (myPos.getRawX() < boxPos.getRawX() + boxSize.getRawX() &&
                            myPos.getRawX() + mySize.getRawX() > boxPos.getRawX() &&
                            myPos.getRawY() < boxPos.getRawY() + boxSize.getRawY() &&
                            myPos.getRawY() + mySize.getRawY() > boxPos.getRawY());
        
        result.hasCollision = hasCollision;
        if (!hasCollision) return result;

        Vector2 myCenterPos = myPos + mySize * 0.5f;
        Vector2 boxCenterPos = boxPos + boxSize * 0.5f;
        Vector2 delta = myCenterPos - boxCenterPos;

        float overlapX = (mySize.getRawX() + boxSize.getRawX()) * 0.5f - std::abs(delta.getRawX());
        float overlapY = (mySize.getRawY() + boxSize.getRawY()) * 0.5f - std::abs(delta.getRawY());

        if (overlapX < overlapY) {
            // Horizontal collision
            result.contactNormal = Vector2{ delta.getRawX() > 0 ? 1.0f : -1.0f, 0.0f };
            float contactX = delta.getRawX() > 0 ? boxPos.getRawX() + boxSize.getRawX() : boxPos.getRawX();
            result.contactPoint = Vector2{ contactX, std::max(myPos.getRawY(), boxPos.getRawY()) };
        }

        else {
            // Vertical collision
            result.contactNormal = Vector2{ 0.0f, delta.getRawY() > 0 ? 1.0f : -1.0f };
            float contactY = delta.getRawY() > 0 ? boxPos.getRawY() + boxSize.getRawY() : boxPos.getRawY();
            result.contactPoint = Vector2{ std::max(myPos.getRawX(), boxPos.getRawX()), contactY };
        }

        return result;
    }
}
