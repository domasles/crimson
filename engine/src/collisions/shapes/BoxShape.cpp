#include <pch.h>

#include <utils/collision.h>

#include <collisions/shapes/BoxShape.h>

namespace engine::collisions::shapes {
    CollisionResult BoxShape::checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const {
        auto result = other.checkCollisionWithBox(otherPos, otherSize, myPos, mySize);
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

    CollisionResult BoxShape::checkCollisionWithCircle(const Vector2& myPos, const Vector2& mySize, const Vector2& circlePos, const Vector2& circleSize) const {
        CollisionResult result;

        Vector2 circleCenter = circlePos + circleSize * 0.5f;
        Vector2 boxCenter = myPos + mySize * 0.5f;
        Vector2 halfExtents = mySize * 0.5f;

        Vector2 closest = Vector2{
            std::max(myPos.getRawX(), std::min(circleCenter.getRawX(), myPos.getRawX() + mySize.getRawX())),
            std::max(myPos.getRawY(), std::min(circleCenter.getRawY(), myPos.getRawY() + mySize.getRawY()))
        };

        // Calculate relative position for ellipse test
        Vector2 diff = circleCenter - closest;
        Vector2 ellipseRadii = circleSize * 0.5f;

        // Normalize difference by ellipse radii to test if point is inside unit circle
        float normalizedX = ellipseRadii.getRawX() > 0.0f ? diff.getRawX() / ellipseRadii.getRawX() : 0.0f;
        float normalizedY = ellipseRadii.getRawY() > 0.0f ? diff.getRawY() / ellipseRadii.getRawY() : 0.0f;
        float distanceSquared = normalizedX * normalizedX + normalizedY * normalizedY;

        result.hasCollision = distanceSquared <= 1.0f;

        if (result.hasCollision) {
            if (distanceSquared > 0.0f) {
                float distance = std::sqrt(distanceSquared);

                result.contactNormal = Vector2{ normalizedX / distance, normalizedY / distance };
                result.contactPoint = closest;
            }

            else {
                Vector2 toCenter = circleCenter - boxCenter;
                Vector2 absToCenter = Vector2{ std::abs(toCenter.getRawX()), std::abs(toCenter.getRawY()) };
                Vector2 overlap = halfExtents - absToCenter;

                if (overlap.getRawX() < overlap.getRawY()) {
                    result.contactNormal = Vector2{ toCenter.getRawX() > 0 ? 1.0f : -1.0f, 0.0f };
                    result.contactPoint = Vector2{ toCenter.getRawX() > 0 ? myPos.getRawX() + mySize.getRawX() : myPos.getRawX(), circleCenter.getRawY() };
                }

                else {
                    result.contactNormal = Vector2{ 0.0f, toCenter.getRawY() > 0 ? 1.0f : -1.0f };
                    result.contactPoint = Vector2{ circleCenter.getRawX(), toCenter.getRawY() > 0 ? myPos.getRawY() + mySize.getRawY() : myPos.getRawY() };
                }
            }
        }

        return result;
    }

    void BoxShape::renderGizmo(SDL_Renderer* renderer, const Vector2& position, const Vector2& size) const {
        SDL_FRect rect = {
            position.getX(),
            position.getY(),
            size.getX(),
            size.getY()
        };

        SDL_RenderRect(renderer, &rect);
    }
}
