#pragma once

#include <collisions/CollisionShape.h>

namespace engine::collisions::shapes {
    class BoxShape : public CollisionShape {
        public:
            Vector2 getSupportPoint(const Vector2& position, const Vector2& size, const Vector2& direction) const override;
            AABB getBoundingBox(const Vector2& position, const Vector2& size) const override;

            void renderGizmo(GLRenderer* renderer, const Vector2& position, const Vector2& size, const Color& color, float rotation = 0.0f) const override;

            std::unique_ptr<CollisionShape> clone() const override { return std::make_unique<BoxShape>(*this); }
    };
}
