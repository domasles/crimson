#include <pch.h>

#include <collisions/shapes/BoxShape.h>

namespace engine::collisions::shapes {
    Vector2 BoxShape::getSupportPoint(const Vector2& position, const Vector2& size, const Vector2& direction) const {
        float x = direction.getRawX() >= 0.0f ? position.getRawX() + size.getRawX() : position.getRawX();
        float y = direction.getRawY() >= 0.0f ? position.getRawY() + size.getRawY() : position.getRawY();

        return Vector2{ x, y };
    }

    AABB BoxShape::getBoundingBox(const Vector2& position, const Vector2& size) const {
        return AABB{ position, position + size };
    }

    void BoxShape::renderGizmo(GLRenderer* renderer, const Vector2& position, const Vector2& size, const Color& color, float rotation) const {
        renderer->drawRect(position, size, color, rotation);
    }
}
