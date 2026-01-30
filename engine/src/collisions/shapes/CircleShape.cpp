#include <pch.h>

#include <utils/collision.h>

#include <collisions/shapes/CircleShape.h>

namespace engine::collisions::shapes {
    CollisionResult CircleShape::checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const {
        auto result = other.checkCollisionWithCircle(otherPos, otherSize, myPos, mySize);
        return result;
    }

    CollisionResult CircleShape::checkCollisionWithBox(const Vector2& myPos, const Vector2& mySize, const Vector2& boxPos, const Vector2& boxSize) const {
        CollisionResult result;

        Vector2 circleCenter = myPos + mySize * 0.5f;
        Vector2 boxCenter = boxPos + boxSize * 0.5f;
        Vector2 halfExtents = boxSize * 0.5f;

        Vector2 closest = Vector2{
            std::max(boxPos.getRawX(), std::min(circleCenter.getRawX(), boxPos.getRawX() + boxSize.getRawX())),
            std::max(boxPos.getRawY(), std::min(circleCenter.getRawY(), boxPos.getRawY() + boxSize.getRawY()))
        };

        // Calculate relative position for ellipse test
        Vector2 diff = circleCenter - closest;
        Vector2 ellipseRadii = mySize * 0.5f;

        // Normalize difference by ellipse radii to test if point is inside unit circle
        float normalizedX = ellipseRadii.getRawX() > 0.0f ? diff.getRawX() / ellipseRadii.getRawX() : 0.0f;
        float normalizedY = ellipseRadii.getRawY() > 0.0f ? diff.getRawY() / ellipseRadii.getRawY() : 0.0f;
        float distanceSquared = normalizedX * normalizedX + normalizedY * normalizedY;

        result.hasCollision = distanceSquared <= 1.0f;

        if (result.hasCollision) {
            if (distanceSquared > 0.0f) {
                float distance = std::sqrt(distanceSquared);

                result.contactNormal = Vector2{ -normalizedX / distance, -normalizedY / distance };
                result.contactPoint = closest;
            }

            else {
                Vector2 toCenter = circleCenter - boxCenter;
                Vector2 absToCenter = Vector2{ std::abs(toCenter.getRawX()), std::abs(toCenter.getRawY()) };
                Vector2 overlap = halfExtents - absToCenter;

                if (overlap.getRawX() < overlap.getRawY()) {
                    result.contactNormal = Vector2{ toCenter.getRawX() > 0 ? 1.0f : -1.0f, 0.0f };
                    result.contactPoint = Vector2{ toCenter.getRawX() > 0 ? boxPos.getRawX() + boxSize.getRawX() : boxPos.getRawX(), circleCenter.getRawY() };
                }

                else {
                    result.contactNormal = Vector2{ 0.0f, toCenter.getRawY() > 0 ? 1.0f : -1.0f };
                    result.contactPoint = Vector2{ circleCenter.getRawX(), toCenter.getRawY() > 0 ? boxPos.getRawY() + boxSize.getRawY() : boxPos.getRawY() };
                }
            }
        }

        return result;
    }

    CollisionResult CircleShape::checkCollisionWithCircle(const Vector2& myPos, const Vector2& mySize, const Vector2& circlePos, const Vector2& circleSize) const {
        CollisionResult result;

        Vector2 myCenter = myPos + mySize * 0.5f;
        Vector2 otherCenter = circlePos + circleSize * 0.5f;

        Vector2 myRadii = mySize * 0.5f;
        Vector2 otherRadii = circleSize * 0.5f;

        // Calculate distance between centers
        Vector2 distance = otherCenter - myCenter;

        // Transform both ellipses to unit circles for easier collision detection
        float normalizedDx = distance.getRawX() / (myRadii.getRawX() + otherRadii.getRawX());
        float normalizedDy = distance.getRawY() / (myRadii.getRawY() + otherRadii.getRawY());
        float normalizedDistanceSquared = normalizedDx * normalizedDx + normalizedDy * normalizedDy;

        result.hasCollision = normalizedDistanceSquared <= 1.0f;

        if (result.hasCollision && normalizedDistanceSquared > 0.0f) {
            float normalizedDistance = std::sqrt(normalizedDistanceSquared);

            result.contactNormal = Vector2{ normalizedDx / normalizedDistance, normalizedDy / normalizedDistance };

            // Contact point is on the surface of my ellipse
            float contactX = myCenter.getRawX() + result.contactNormal.getRawX() * myRadii.getRawX();
            float contactY = myCenter.getRawY() + result.contactNormal.getRawY() * myRadii.getRawY();

            result.contactPoint = Vector2{ contactX, contactY };
        }

        return result;
    }

    void CircleShape::renderGizmo(SDL_Renderer* renderer, const Vector2& position, const Vector2& size) const {
        const int segments = 16;
        const float PI = 3.14159265359f;

        float centerX = position.getX() + size.getX() * 0.5f;
        float centerY = position.getY() + size.getY() * 0.5f;

        float radiusX = size.getX() * 0.5f;
        float radiusY = size.getY() * 0.5f;

        for (int i = 0; i < segments; ++i) {
            float angle1 = (2.0f * PI * i) / segments;
            float angle2 = (2.0f * PI * (i + 1)) / segments;

            float x1 = centerX + radiusX * cos(angle1);
            float y1 = centerY + radiusY * sin(angle1);
            float x2 = centerX + radiusX * cos(angle2);
            float y2 = centerY + radiusY * sin(angle2);

            SDL_RenderLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
        }
    }
}